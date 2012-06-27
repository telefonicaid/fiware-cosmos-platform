"""
HUE automatic deployment
"""
import os
import shutil
from StringIO import StringIO

from fabric.api import run, put, cd, env, sudo
from fabric.colors import red, yellow
import fabric.context_managers as ctx
from fabric.contrib import files
from fabric.decorators import roles
from fabric.utils import puts, warn
from mako.template import Template

import common

BASEPATH = os.path.dirname(os.path.realpath(__file__))

def install_git():
    git_version_string = run('git --version')
    version_string = git_version_string.split()[-1]
    version_numbers = map(int, version_string.split('.'))
    if version_numbers[0] < 1 or version_numbers[1] < 7 or\
        version_numbers[2] < 10:
        git_repo = 'http://pkgs.repoforge.org/git/'
        git_pkg = 'git-1.7.10.4-1.el6.rfx.x86_64.rpm'
        git_url = git_repo + git_pkg
        run('wget %s' % git_url)
        run('rpm -Uvh --force --nodeps git-1.7.10.4-1.el6.rfx.x86_64.rpm')
        run('rm -f git-1.7.10.4-1.el6.rfx.x86_64.rpm')

def install_and_patch_hue(config):
    common.install_cdh_repo(config)
    local_patch_path = os.path.join(BASEPATH, config['hue_patch_dir'],
                                    config['hue_patch_name'])
    remote_patch_path = os.path.join('~', config['hue_patch_name'])
    with ctx.hide('stdout'):
        hue_pkgs = ["hue-common", "hue-filebrowser", "hue-help",
                    "hue-jobbrowser", "hue-jobsub", "hue-plugins",
                    "hue-proxy", "hue-server", "hue-shell"]
        for pkg_name in hue_pkgs:
            run("yum -y erase {0}".format(pkg_name))
        run("yum -y install hue")
        put(local_patch_path, remote_patch_path)
        with cd("/usr/share/hue"):
            run("git apply -p2 --reject {0}".format(remote_patch_path))
    common.add_iptables_rule('INPUT -p tcp -m tcp --dport 80 -j ACCEPT')
    common.add_iptables_rule('INPUT -p tcp -m tcp --dport 8001 -j ACCEPT')
    common.add_iptables_rule('INPUT -p tcp -m tcp --dport 8002 -j ACCEPT')
    common.add_iptables_rule('INPUT -p tcp -m tcp --dport 8003 -j ACCEPT')
    
    hueconf = StringIO()
    template = Template(filename = os.path.join(BASEPATH,
                                                'templates/hue.ini.mako'))
    hueconf.write(template.render(
            jobtracker = config['hosts']['jobtracker'][0],
            namenode = config['hosts']['namenode'][0]))
    put(hueconf, '/etc/hue/hue.ini')
    sudo('hadoop dfs -mkdir /user/hive/warehouse', user='hdfs')
    sudo('hadoop dfs -chown -R hive /user/hive/', user='hdfs')
    sudo('hadoop dfs -mkdir /tmp', user='hdfs')
    sudo('hadoop dfs -chmod +777 /tmp', user='hdfs')
    sudo('chown -R hue /var/lib/hive/')

@roles('namenode', 'jobtracker', 'datanodes', 'tasktrackers')
def install_hue_plugins():
    env.port = '22'
    run("yum -y install hue-plugins")

def install_thrift(thrift_tarpath):
    if files.exists('/usr/local/bin/thrift'):
        puts("Thrift already installed, skipping ...")
    else:
        puts("Installing thrift")
        with ctx.hide('stdout'):
            run(("yum -y install automake libtool flex bison pkgconfig "
                 "gcc-c++ boost-devel libevent-devel zlib-devel python-devel "
                 "ruby-devel openssl-devel"))
            put(thrift_tarpath)
            run("tar xfz thrift-0.8.0.tar.gz")
            with cd("thrift-0.8.0"):
                run("./configure")
                run("make")
                run("make install")
               
def install_cosmos_app():
    cosmos_app_install_path = '/usr/share/hue/apps/cosmos/'
    if files.exists(cosmos_app_install_path):
        puts("Found an existing installation of Cosmos app!")
        puts("Going to remove it")
        run("rm -rf {0}".format(cosmos_app_install_path))
    if files.exists("cosmos-app"):
        run("rm -rf cosmos-app")
    run("mkdir cosmos-app")
    local_cosmos_app = os.path.join(BASEPATH,
                                "../cosmos/platform/frontend/hue-apps/cosmos")
    if os.path.exists(os.path.join(local_cosmos_app, "parts")):
        warn(yellow("Project root was built with buildout"))
        warn(yellow("Going to remove all buildout by-products"))
        os.remove(os.path.join(local_cosmos_app, ".installed.cfg"))
        shutil.rmtree(os.path.join(local_cosmos_app, "bin"))
        shutil.rmtree(os.path.join(local_cosmos_app, "develop-eggs"))
        shutil.rmtree(os.path.join(local_cosmos_app, "eggs"))
        shutil.rmtree(os.path.join(local_cosmos_app, "parts"))
    put(local_cosmos_app, "cosmos-app")
    with cd("cosmos-app/cosmos"):
        puts("About to run buildout")
        run("python bootstrap.py")
        run("bin/buildout -c buildout.prod.cfg")

def start_daemons():
    with cd("/etc/init.d"):
        run("./hue start")

def cleanup():
    patch = "hue-patch-cdh3u4-r0.4.diff"
    if files.exists(patch):
        run("rm {0}".format(patch))
    if files.exists("cosmos-app"):
        run("rm -rf cosmos-app")
