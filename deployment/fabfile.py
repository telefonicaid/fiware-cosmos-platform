"""
Cosmos automatic deployment Fabric file -

causes Fabric to deploy each Cosmos component at a configured host.
"""
import json
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
    execute(deploy_cdh)
    execute(deploy_namenode_daemon)
    execute(deploy_jobtracker_daemon)
    execute(deploy_datanode_daemon)
    execute(deploy_tasktracker_daemon)

@roles('namenode')
def deploy_hue():
    """Deploy the HUE frontend from Clouder, plus our fixes and our apps"""
    pdihub = CONFIG['github']
    checkout_dir = CONFIG['hue_checkout']
    run("yum install hue") # at version 1.2.0.0+114.35
    run("yum install git")
    run("git clone {0}/HUE {1}".format(pdihub, checkout_dir))
    #run("git apply <hue-fixes> <hue>")
#     put("./cosmos/platform/frontend/hue/app/cosmos")
    # remote cd ./cosmos/platform/frontend/hue-app/cosmos
#     run("python bootstrap.py")
#     run("bin/buildout -c buildout.prod.cfg")
#     run("/etc/init.d/frontend start")

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

def install_cdh():
    """Install the latest Hadoop distribution in CDH3"""
    run('wget http://archive.cloudera.com/redhat'
        '/cdh/cdh3-repository-1.0-1.noarch.rpm')
    run('rpm -Uvh --force cdh3-repository-1.0-1.noarch.rpm')
    run('rpm --import '
        'http://archive.cloudera.com/redhat/cdh/RPM-GPG-KEY-cloudera')
    run('yum -y install hadoop-0.20 hadoop-0.20-native')

def create_hadoop_dirs():
    """Create necessary directories for Hadoop"""
    run('rm -rf /data1')
    run('mkdir /data1')
    run('mkdir -m 700 /data1/data')
    run('chown hdfs:hadoop /data1/data')
    run('mkdir -m 755 /data1/mapred')
    run('chown mapred:hadoop /data1/mapred')    
    run('mkdir -m 755 /data1/name')
    run('chown hdfs:hadoop /data1/name')
    
def configure_hadoop():
    """Generate  Hadoop configuration files"""
    with cd('/etc/hadoop/conf'):
        coresite = StringIO()
        template = Template(filename='templates/core-site.mako')
        coresite.write(template.render(namenode=CONFIG['hosts']['namenode'][0]))
        put(coresite, 'core-site.xml')
        
        masters = StringIO()
        for master in set(CONFIG['hosts']['namenode']
                            + CONFIG['hosts']['jobtracker']):
            masters.write('%s\n' % master)
        put(masters, 'masters')
        
        slaves = StringIO()
        for slave in set(CONFIG['hosts']['datanodes']
                            + CONFIG['hosts']['tasktrackers']):
            slaves.write('%s\n' % slave)
        put(slaves, 'slaves')
            
        mapredsite = StringIO()
        template = Template(filename='templates/mapred-site.mako')
        mapredsite.write(template.render(
                jobtracker=CONFIG['hosts']['jobtracker'][0]))
        put(mapredsite, 'mapred-site.xml')
        
        hdfssite = StringIO()
        template = Template(filename='templates/hdfs-site.mako')
        hdfssite.write(template.render())
        put(hdfssite, 'hdfs-site.xml')
       
@roles('namenode', 'jobtracker', 'datanodes', 'tasktrackers')
def deploy_cdh():
    """Deploys the CDH distribution of Hadoop:
        - Installs Hadoop
        - Configures Hadoop"""
    install_cdh()
    create_hadoop_dirs()
    configure_hadoop()
    
def deploy_daemon(daemon):
    """Deploys a Hadoop daemon"""
    daemon_path = '/etc/init.d/hadoop-0.20-%s' % daemon
    if files.exists(daemon_path):
        run('%s stop' % daemon_path)
    run('yum -y install hadoop-0.20-%s' % daemon)

def start_daemon(daemon):
    """Starts a Hadoop daemon"""
    run('/etc/init.d/hadoop-0.20-%s start' % daemon)

@roles('datanodes')
def deploy_datanode_daemon():
    """Deploys the datanode Hadoop daemon"""
    deploy_daemon('datanode')
    start_daemon('datanode')

@roles('namenode')
def deploy_namenode_daemon():
    """Deploys the namenode Hadoop daemon"""
    deploy_daemon('namenode')
    sudo('yes Y | hadoop namenode -format', user='hdfs')
    start_daemon('namenode')
    sudo('hadoop dfs -mkdir /hadoop/mapred/system', user='hdfs')
    sudo('hadoop dfs -chown -R mapred /hadoop/mapred/', user='hdfs')

@roles('jobtracker')
def deploy_jobtracker_daemon():
    """Deploys the jobtracker Hadoop daemon"""
    deploy_daemon('jobtracker')
    start_daemon('jobtracker')

@roles('tasktrackers')    
def deploy_tasktracker_daemon():
    """Deploys the tasktracker Hadoop daemon"""
    deploy_daemon('tasktracker')
    start_daemon('tasktracker')
  
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
