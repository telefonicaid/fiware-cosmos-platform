"""
HUE automatic deployment
"""
import os
from fabric.api import run, put, cd, env
import fabric.context_managers as ctx
from fabric.contrib import files
from fabric.decorators import roles
from fabric.utils import puts

def patch_hue(config):
    local_patch_path = os.path.join(config['hue_patch_dir'],
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
    put("../cosmos/platform/frontend/hue-apps/cosmos", "cosmos-app")
    with cd("cosmos-app/cosmos"):
        puts("About to run buildout")
        ## TODO: check the python version before running
        run("python2.6 bootstrap.py")
        run("bin/buildout -c buildout.prod.cfg")

def start_daemons():
    with cd("/etc/init.d"):
        put("templates/hue.init", "hue")
        run("./hue start")

def cleanup():
    patch = "hue-patch-cdh3u4-r0.4.diff"
    if files.exists(patch):
        run("rm {0}".format(patch))
    if files.exists("cosmos-app"):
        run("rm -rf cosmos-app")
