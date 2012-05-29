"""
Cosmos automatic deployment Fabric file -

causes Fabric to deploy each Cosmos component at a configured host.
"""
import json
from fabric.api import *
from fabric.decorators import *
from fabric.contrib import files
from StringIO import StringIO
from mako.template import Template

DEFAULT_CONFIG = 'staging.json'

config = json.loads(open(DEFAULT_CONFIG, 'r').read())
env.user = config['user'] 
env.password = config['password']
env.roledefs = config['hosts']

def deploy():
    execute(deploy_cdh)
    execute(deploy_namenode_daemon)
    execute(deploy_jobtracker_daemon)
    execute(deploy_datanode_daemon)
    execute(deploy_tasktracker_daemon)

@roles('namenode') # frontend
def deploy_hue():
    """Deploy the HUE frontend from Cloudera, plus our fixes and our apps"""
    pdihub = config['github']
    checkout_dir = config['hue_checkout']
    run("yum install hue") # at version 1.2.0.0+114.35
    run("yum install git")
    run("git clone git@pdihub.hi.inet:Cosmos/HUE.git {0}".format(checkout_dir))
    run("git apply {0} {1}".format(checkout_dir, "/tmp"))
#     put("./cosmos/platform/frontend/hue/app/cosmos")
    # with cd ./cosmos/platform/frontend/hue-app/cosmos
#     run("python bootstrap.py")
#     run("bin/buildout -c buildout.prod.cfg")
#     run("/etc/init.d/frontend start")

@roles('namenode') # frontend
def deploy_sftp():
    """Deploys the SFTP server as a Java JAR and starts it"""
    with cd("/root/injection"):
        put("target/injection*.jar")
        injection_conf = StringIO()
        template = Template(filename='injection.conf.mako')
        injection_conf.write(template.render(namenode=config['hosts']['namenode'][0]))
        put(injection_conf, "/etc/injection.cfg")
        put("templates/injection.init.d", "/etc/init.d/injection")
        #run("update_config ?")
    run("/etc/init.d/injection start")

def install_cdh():
    """Install the latest Hadoop distribution in CDH3"""
    run('wget http://archive.cloudera.com/redhat/cdh/cdh3-repository-1.0-1.noarch.rpm')
    run('rpm -Fvh cdh3-repository-1.0-1.noarch.rpm')
    run('rpm --import http://archive.cloudera.com/redhat/cdh/RPM-GPG-KEY-cloudera')
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
        coresite.write(template.render(namenode=config['hosts']['namenode'][0]))
        put(coresite, 'core-site.xml')
        
        masters = StringIO()
        for master in set(config['hosts']['namenode'] + config['hosts']['jobtracker']):
            masters.write('%s\n' % master)
        put(masters, 'masters')
        
        slaves = StringIO()
        for slave in set(config['hosts']['datanodes'] + config['hosts']['tasktrackers']):
            slaves.write('%s\n' % slave)
        put(slaves, 'slaves')
            
        mapredsite = StringIO()
        template = Template(filename='templates/mapred-site.mako')
        mapredsite.write(template.render(jobtracker=config['hosts']['jobtracker'][0]))
        put(mapredsite, 'mapred-site.xml')
        
        hdfssite = StringIO()
        template = Template(filename='templates/hdfs-site.mako')
        hdfssite.write(template.render())
        put(hdfssite, 'hdfs-site.xml')
       
@roles('namenode', 'jobtracker', 'datanodes', 'tasktrackers')
def deploy_cdh():
    install_cdh()
    create_hadoop_dirs()
    configure_hadoop()
    
def deploy_daemon(daemon):
    daemonPath = '/etc/init.d/hadoop-0.20-%s' % daemon
    if files.exists(daemonPath):
        run('%s stop' % daemonPath)
    run('yum -y install hadoop-0.20-%s' % daemon)

def start_daemon(daemon):
    run('/etc/init.d/hadoop-0.20-%s start' % daemon)

@roles('datanodes')
def deploy_datanode_daemon():
    deploy_daemon('datanode')
    start_daemon('datanode')

@roles('namenode')
def deploy_namenode_daemon():
    deploy_daemon('namenode')
    sudo('yes Y | hadoop namenode -format', user='hdfs')
    start_daemon('namenode')
    sudo('hadoop dfs -mkdir /hadoop/mapred/system', user='hdfs')
    sudo('hadoop dfs -chown -R mapred /hadoop/mapred/', user='hdfs')

@roles('jobtracker')
def deploy_jobtracker_daemon():
    deploy_daemon('jobtracker')
    start_daemon('jobtracker')

@roles('tasktrackers')    
def deploy_tasktracker_daemon():
    deploy_daemon('tasktracker')
    start_daemon('tasktracker')
  
@roles('mongo')  
def deploy_mongo():
    pass

def deploy_models():
    pass
