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
import iptables


BASEPATH = os.path.dirname(os.path.realpath(__file__))

REQUIRED_GIT_MAJOR_VERSION = 1
REQUIRED_GIT_MINOR_VERSION = 7
REQUIRED_GIT_RELEASE_VERSION = 10

def install_git():
    """
    Installs GIT from a non-standard repository, because standard versions
    of GIT for our current OS can be very outdated.
    """
    git_version_string = run('git --version')
    version_string = git_version_string.split()[-1]
    version_numbers = map(int, version_string.split('.'))
    if version_numbers[0] < REQUIRED_GIT_MAJOR_VERSION or\
            version_numbers[1] < REQUIRED_GIT_MINOR_VERSION or\
            version_numbers[2] < REQUIRED_GIT_RELEASE_VERSION:
        git_repo = 'http://pkgs.repoforge.org/git/'
        git_pkg = 'git-1.7.10.4-1.el6.rfx.x86_64.rpm'
        git_url = git_repo + git_pkg
        run('wget %s' % git_url)
        run('rpm -Uvh --force --nodeps git-1.7.10.4-1.el6.rfx.x86_64.rpm')
        run('rm -f %s' % git_pkg)

def install_and_patch_hue(config):
    """
    Installs HUE from the Cloudera-provided RPM, and then patches the installed
    sources and binaries with Cosmos behaviour and look.
    """
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
    iptables.accept_in_tcp(80)
    iptables.accept_in_tcp(8001)
    iptables.accept_in_tcp(8002)
    iptables.accept_in_tcp(8003)

    common.instantiate_template('templates/hue.ini.mako', '/etc/hue/hue.ini',
                                context=dict(
                                    jobtracker = config['hosts']['jobtracker'][0],
                                    namenode = config['hosts']['namenode'][0]))

    sudo('hadoop dfs -mkdir /user/hive/warehouse', user='hdfs')
    sudo('hadoop dfs -chown -R hive /user/hive/', user='hdfs')
    sudo('hadoop dfs -chmod +777 /user/hive/warehouse', user='hdfs')
    sudo('hadoop dfs -mkdir /tmp', user='hdfs')
    sudo('hadoop dfs -chmod +777 /tmp', user='hdfs')
    sudo('chown -R hue /var/lib/hive/')

@roles('namenode', 'jobtracker', 'datanodes', 'tasktrackers')
def install_hue_plugins():
    """
    Installs HUE plugins from Cloudera-provided packages
    """
    env.port = '22'
    run("yum -y install hue-plugins")

def install_thrift(thrift_tarpath):
    """
    Installs Thrift from sources, only if not already present in the system.
    """
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
    """
    Install the Cosmos app into HUE, by registering it as a new app. The Cosmos
    app has a buildout script that takes care of the registration process.
    """
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
    """
    Start HUE daemons, including jobsubd
    """
    with cd("/etc/init.d"):
        run("./hue start")

def cleanup():
    """
    Clean up uploaded files and directories
    """
    patch = "hue-patch-cdh3u4-r0.4.diff"
    if files.exists(patch):
        run("rm {0}".format(patch))
    if files.exists("cosmos-app"):
        run("rm -rf cosmos-app")
