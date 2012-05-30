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

def deploy():
    """Deploys all the necessary components to get a running Cosmos cluster"""
    deploy_cdh()

def has_package(pkg):
    with ctx.hide('stdout'):
        out = run('yum list -q installed | grep ^%s\\. || echo' % pkg)
        return len(out.strip()) > 0

@task
@roles('frontend') # namenode
def deploy_hue():
    """Deploy the HUE frontend from Cloudera, plus our fixes and our app"""
    pdihub = CONFIG['github']
    checkout_dir = CONFIG['hue_checkout']
    hue_patch_path = "/root/hue-patch-cdh3u4-r0.4.diff"
    has_hue = has_package('hue')
    ## TODO: do the right thing without confirm
    if has_hue and confirm('HUE already installed. Reinstall?'):
        with ctx.hide('stdout'):
            ## TODO: convert to explicit list of pkgs?
            run(("for pkg in `yum -q list installed | grep hue | awk '{print "
                 "$1}'`;do yum -y erase $pkg; done"))
        run("yum -y install hue") # at version 1.2.0.0+114.35
        ## TODO: extract path
        put("../cosmos/platform/frontend/hue-patches/hue-patch-cdh3u4-r0.4.diff", 
            hue_patch_path)
        with cd("/usr/share/hue"):
            run("git apply -p2 --reject {0}".format(hue_patch_path))
    if not has_package('git'):
        run("yum -y install git")
    if not has_package('mysql'):
        run("yum -y install mysql")
    if not files.exists('/usr/local/bin/thrift'):
        puts("installing thrift")
        with ctx.hide('stdout'):
            run(("yum -y install automake libtool flex bison pkgconfig "
                 "gcc-c++ boost-devel libevent-devel zlib-devel python-devel "
                 "ruby-devel openssl-devel"))
            ## TODO: extract path
            put("~/Applications/install-dependencies/thrift-0.8.0.tar.gz")
            run("tar xfz thrift-0.8.0.tar.gz")
            with cd("thrift-0.8.0"):
                run("./configure")
                run("make")
                run("make install")
    put("../cosmos/platform/frontend/hue-apps/cosmos", "cosmos-app")
    with cd("cosmos-app/cosmos"):
        puts("About to run buildout")
        run("python2.6 bootstrap.py")
        run("bin/buildout -c buildout.prod.cfg")
    ## TODO: cleanup

@task
@roles('namenode') # frontend
def deploy_sftp():
    """Deploys the SFTP server as a Java JAR and starts it"""
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
    """
    Deploys the CDH distribution of Hadoop:

    - Installs Hadoop
    - Configures Hadoop
    """
    execute(hadoop_install.install_cdh)
    execute(hadoop_install.create_hadoop_dirs)
    execute(hadoop_install.configure_hadoop, CONFIG)
    execute(hadoop_install.deploy_namenode_daemon)
    execute(hadoop_install.deploy_jobtracker_daemon)
    execute(hadoop_install.deploy_datanode_daemon)
    execute(hadoop_install.deploy_tasktracker_daemon)
  
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
