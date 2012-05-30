"""
Cosmos automatic deployment Fabric file -

causes Fabric to deploy each Cosmos component at a configured host.
"""
import os
import json
import hadoop_install
from fabric.api import run, execute, sudo, put, cd, env
from fabric.decorators import roles
from fabric.contrib import files
from StringIO import StringIO
from mako.template import Template

DEFAULT_CONFIG = 'staging.json'

CONFIG = json.loads(open(DEFAULT_CONFIG, 'r').read())
env.user = CONFIG['user'] 
env.password = CONFIG['password']
env.roledefs = CONFIG['hosts']

def deploy():
    """Deploys all the necessary components to get a running Cosmos cluster"""
    deploy_cdh()

def has_package(pkg):
    out = run("rpm -qa | grep {}".format(pkg))
    return len(out.strip()) > 0

def has_hue():
    return has_package('hue')

@roles('frontend') # namenode
def deploy_hue():
    """Deploy the HUE frontend from Cloudera, plus our fixes and our apps"""
    pdihub = CONFIG['github']
    checkout_dir = CONFIG['hue_checkout']
    # Remaining Dependencies: MySQL, thrift
    run("yum -y erase hue*")
    run("yum -y install hue") # at version 1.2.0.0+114.35
    run("yum -y install git")
    put("hue-patch-cdh3u4-r0.4.diff", "/root/")
    with cd("/usr/share/hue"):
        run("git apply -p2 --reject {0}".format(os.path.join(
                        checkout_dir, 'cosmos', 'platform', 'frontend',
                        'hue-patches', 'hue-patch-cdh3u4-r0.4.diff' )))
    #run("rm -rf {0}".format(checkout_dir))
#     put("./cosmos/platform/frontend/hue/app/cosmos")
    # with cd ./cosmos/platform/frontend/hue-app/cosmos
#     run("hue.py")

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
      
def deploy_cdh():
    """Deploys the CDH distribution of Hadoop:
        - Installs Hadoop
        - Configures Hadoop"""
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

def deploy_models():
    """ This function is not ready for production. It is only here as a general
    idea on what is needed to deploy a model, but we currently have no models to
    deploy"""
    modelpaths = [] # Dummy variable, this should be part of
                    # the configuration or similar
    for model in modelpaths:
        put(model)
        sudo('hadoop dfs -put {0} /models/{0}'.format(model))
        run('rm %s' % model)
