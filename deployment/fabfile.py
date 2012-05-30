"""
Cosmos automatic deployment Fabric file -

causes Fabric to deploy each Cosmos component at a configured host.
"""
import os
import json
from StringIO import StringIO

from fabric.api import run, execute, sudo, put, cd, env, local
import fabric.context_managers as ctx
from fabric.contrib import files
from fabric.contrib.console import confirm
from fabric.decorators import roles, task
from fabric.utils import puts
from mako.template import Template

import hadoop_install

DEFAULT_CONFIG = 'staging.json'

CONFIG = json.loads(open(DEFAULT_CONFIG, 'r').read())
env.user = CONFIG['user'] 
env.password = CONFIG['password']
env.roledefs = CONFIG['hosts']

@task
def deploy():
    """
    Deploys all the necessary components to get a running Cosmos cluster
    """
    deploy_cdh()
    deploy_hue()
    deploy_models()
    deploy_sftp()

def has_package(pkg):
    with ctx.hide('stdout'):
        out = run('yum list -q installed | grep ^%s\\. || echo' % pkg)
        return len(out.strip()) > 0

def patch_hue():
    ## TODO: extract to config
    local_patch_path = "../cosmos/platform/frontend/hue-patches/hue-patch-cdh3u4-r0.4.diff"
    remote_patch_path = "hue-patch-cdh3u4-r0.4.diff"
    with ctx.hide('stdout'):
        hue_pkgs = ["hue-common", "hue-filebrowser", "hue-help",
                    "hue-jobbrowser", "hue-jobsub", "hue-plugins",
                    "hue-proxy", "hue-server", "hue-shell"]
        for pkg_name in hue_pkgs:
            import ipdb; ipdb.set_trace()
            run("yum erase -y {}".format(pkg_name))
        run("yum -y install hue")
        put(local_patch_path, remote_patch_path)
        with cd("/usr/share/hue"):
            run("git apply -p2 --reject {0}".format(remote_patch_path))

def install_thrift(thrift_tarpath):
    if not files.exists('/usr/local/bin/thrift'):
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
    run("mkdir cosmos-app")
    put("../cosmos/platform/frontend/hue-apps/cosmos", "cosmos-app")
    with cd("cosmos-app/cosmos"):
        puts("About to run buildout")
        run("python2.6 bootstrap.py")
        run("bin/buildout -c buildout.prod.cfg")

def check_dependencies(pkg_list):
    """
    Checks that a list of dependencies is met using the OS package manager
    """
    for pkg_name in pkg_list:
        if not has_package(pkg_name):
            run("yum -y install {}".format(pkg_name))

@task
@roles('frontend')
def deploy_hue(thrift_tarpath='~/install-dependencies/thrift-0.8.0.tar.gz'):
    """
    Deploys the HUE frontend from Cloudera, plus our fixes and our app
    """
    check_dependencies(['mysql', 'git'])
    patch_hue()
    install_thrift(thrift_tarpath)
    install_cosmos_app()
    run("rm hue-patch-cdh3u4-r0.4.diff")
    run("rm -rf cosmos-app")

@task
@roles('frontend')
def deploy_sftp():
    """
    Deploys the SFTP server as a Java JAR and starts it
    """
    with cd("/root/injection"):
        put("target/injection*.jar")
        injection_conf = StringIO()
        template = Template(filename='injection.conf.mako')
        injection_conf.write(template.render(
                namenode=CONFIG['hosts']['namenode'][0]))
        put(injection_conf, "/etc/injection.cfg")
        put("templates/injection.init.d", "/etc/init.d/injection")
        #run("update_config ?")
    run("/etc/init.d/injection start")
      
@task
def deploy_cdh():
    """Deploys the Cloudera Distribution for Hadoop"""
    execute(hadoop_install.install_cdh)
    execute(hadoop_install.create_hadoop_dirs)
    execute(hadoop_install.configure_hadoop, CONFIG)
    execute(hadoop_install.deploy_namenode_daemon)
    execute(hadoop_install.deploy_jobtracker_daemon)
    execute(hadoop_install.deploy_datanode_daemon)
    execute(hadoop_install.deploy_tasktracker_daemon)
  
@task
@roles('mongo')  
def deploy_mongo():
    """Install the latest MongoDB distribution"""
    with cd('/etc/yum.repos.d'):
        if not files.exists('10gen.repo'):
            put('templates/10gen.repo', '10gen.repo')
    run('yum -y install mongo-10gen mongo-10gen-server')
    run('service mongod start')
    run('chkconfig mongod on')
    run("mongo --eval \"db.adminCommand('listDatabases').databases.forEach("
        "function (d) { "
        "  if (d.name != 'local' && d.name != 'admin' && d.name != 'config')"
        "    db.getSiblingDB(d.name).dropDatabase();"
        "})\"")

@task
def deploy_models():
    """
    Deploys the preconfigured statistical models

    This function is not ready for production. It is only here as a general
    idea on what is needed to deploy a model, but we currently have no models
    to deploy
    """
    modelpaths = [] # Dummy variable, this should be part of
                    # the configuration or similar
    for model in modelpaths:
        put(model)
        sudo('hadoop dfs -put {0} /models/{0}'.format(model))
        run('rm %s' % model)
