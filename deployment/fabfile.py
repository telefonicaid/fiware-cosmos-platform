from fabric.api import *
from tempfile import TemporaryFile

env.hosts = ['10.95.101.201:2222']
env.user = 'root'
env.password = 'admintid33'

def deploy_hue():
    run("yum install hue")
    run("git clone <pdi hue-fixes> <checkout dir>")
    run("git apply <hue-fixes> <hue>")
    run("scp ./cosmos/platform/frontend/hue/app/cosmos")
    # remote cd ./cosmos/platform/frontend/hue-app/cosmos
    run("python bootstrap.py")
    run("bin/buildout -c buildout.prod.cfg")
    run("/etc/init.d/frontend start")

def deploy_sftp():
    # Jenkins builds JAR
    run("scp target/* /root/injection")
    run("cat template.ini >> /root/injection/server.conf")
    run("update_config ?")
    run("/etc/init.d/injection start")
    with lcd("../../cosmos/platform/injection/server/"):
        local("mvn package")
        put("target/injection*.jar ~/injection")

def deploy_cdh():
    # Install the latest Hadoop distribution in CDH3
    run('wget http://archive.cloudera.com/redhat/cdh/cdh3-repository-1.0-1.noarch.rpm')
    run('rpm -ivh cdh3-repository-1.0-1.noarch.rpm')
    run('rpm -ivh cdh3-repository-1.0-1.noarch.rpm')
    run('rpm --import http://archive.cloudera.com/redhat/cdh/RPM-GPG-KEY-cloudera')
    run('yum install hadoop-0.20 hadoop-0.20-native')
    # Create necessary directories for Hadoop
    run('rm -rf /data1')
    run('mkdir -m 700 /data1/data')
    run('chown hdfs:hadoop /data1/data')
    run('mkdir -m 755 /data1/mapred')
    run('chown mapred:hadoop /data1/mapred')    
    run('mkdir -m 755 /data1/name')
    run('chown hdfs:hadoop /data1/hdfs')
    # Configure Hadoop
    with(TemporaryFile() as core-site,
         TemporaryFile() as masters,
         TemporaryFile() as slaves,
         TemporaryFile() as mapred-site,
         cd('/etc/hadoop/conf')):        
        # Create core-site
        # Create masters
        # Create slaves
        # Create mapred-site
        put(core-site)
        put(masters)
        put(slaves)
        put(mapred-site)
    
def deploy_daemon(daemon):
    run('yum install hadoop-0.20-%s' % daemon)
    run('/etc/init.d/hadoop-0.20-%s start' % daemon)
    
def deploy_datanode_daemon():
    deploy_daemon('datanode')
    
def deploy_namenode_daemon():
    deploy_daemon('namenode')
    
def deploy_jobtracker_daemon():
    deploy_daemon('jobtracker')
    
def deploy_tasktracker_daemon():
    deploy_daemon('tasktracker')
    
def deploy_mongo():
    pass

def deploy_models():
    pass
