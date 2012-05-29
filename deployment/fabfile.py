import json
from fabric.api import *
from fabric.contrib import files
from tempfile import TemporaryFile

DEFAULT_CONFIG = 'staging1.json'

config = json.loads(open(DEFAULT_CONFIG, 'r').read())
env.hosts = config['hosts']
env.user = config['user'] 
env.password = config['password']

# Dummy code, need to remove before submitting
from collections import namedtuple
Config = namedtuple('Config', 'namenode, jobtracker, masters, slaves')
conf = Config('1', '2', ['1', '2'], ['1', '2'])
# End of dummy code

def deploy_hue():
    pdihub = config['github']
    checkout_dir = config['hue_checkout']
    run("yum install hue")
    run("git clone {0}/HUE {1}".format(pdihub, checkout_dir))
    #run("git apply <hue-fixes> <hue>")
#     put("./cosmos/platform/frontend/hue/app/cosmos")
    # remote cd ./cosmos/platform/frontend/hue-app/cosmos
#     run("python bootstrap.py")
#     run("bin/buildout -c buildout.prod.cfg")
#     run("/etc/init.d/frontend start")

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
    run('rpm -Fvh cdh3-repository-1.0-1.noarch.rpm')
    run('rpm --import http://archive.cloudera.com/redhat/cdh/RPM-GPG-KEY-cloudera')
    run('yum -y install hadoop-0.20 hadoop-0.20-native')
    # Create necessary directories for Hadoop
    run('rm -rf /data1')
    run('mkdir /data1')
    run('mkdir -m 700 /data1/data')
    run('chown hdfs:hadoop /data1/data')
    run('mkdir -m 755 /data1/mapred')
    run('chown mapred:hadoop /data1/mapred')    
    run('mkdir -m 755 /data1/name')
    run('chown hdfs:hadoop /data1/name')
    # Configure Hadoop
    with cd('/etc/hadoop/conf'):
        with TemporaryFile() as coresite:
            coresite.write('<?xml version="1.0">\n')
            coresite.write('<?xml-stylesheet type="text/xsl" href="configuration.xsl"?>\n')
            coresite.write('<configuration>\n')
            coresite.write('  <property>\n')
            coresite.write('    <name>fs.default.name</name>\n')
            coresite.write('    <value>hdfs://%s</value>\n' % conf.namenode)
            coresite.write('  </property>\n')
            coresite.write('</configuration>\n')
            put(coresite, 'core-site.xml')
        with TemporaryFile() as masters:
            for master in conf.masters:
                masters.write('%s\n' % master)
            put(masters, 'masters')
        with TemporaryFile() as slaves:
            for slave in conf.slaves:
                slaves.write('%s\n' % slave)
            put(slaves, 'slaves')
        with TemporaryFile() as mapredsite:
            mapredsite.write('<?xml version="1.0">\n')
            mapredsite.write('<?xml-stylesheet type="text/xsl" href="configuration.xsl"?>\n')
            mapredsite.write('<configuration>\n')
            mapredsite.write('  <property>\n')        
            mapredsite.write('    <name>mapred.job.tracker</name>\n')
            mapredsite.write('    <value>%s</value>\n' % conf.jobtracker)
            mapredsite.write('  </property>\n')
            mapredsite.write('  <property>\n')        
            mapredsite.write('    <name>mapred.system.dir</name>\n')
            mapredsite.write('    <value>/hadoop/mapred/system</value>\n')
            mapredsite.write('  </property>\n')
            mapredsite.write('  <property>\n')        
            mapredsite.write('    <name>mapred.local.dir</name>\n')
            mapredsite.write('    <value>/data1/mapred</value>\n')
            mapredsite.write('  </property>\n')
            mapredsite.write('  <property>\n')        
            mapredsite.write('    <name>mapreduce.jobtracker.staging.root.dir</name>\n')
            mapredsite.write('    <value>/user</value>\n')
            mapredsite.write('  </property>\n')
            mapredsite.write('  <!-- optional settings -->\n')
            mapredsite.write('  <property>\n')
            mapredsite.write('    <name>mapred.child.java.opts</name>\n')
            mapredsite.write('    <value>-Xmx350m</value>\n')
            mapredsite.write('  </property>\n')
            mapredsite.write('  <property>\n')        
            mapredsite.write('    <name>mapred.child.ulimit</name>\n')
            mapredsite.write('    <value>420m</value>\n')
            mapredsite.write('  </property>\n')
            mapredsite.write('  <!-- HUE integration -->\n')
            mapredsite.write('  <property>\n')        
            mapredsite.write('    <name>jobtracker.thrift.address</name>\n')
            mapredsite.write('    <value>0.0.0.0:9290</value>\n')
            mapredsite.write('  </property>\n')
            mapredsite.write('  <property>\n')        
            mapredsite.write('    <name>mapred.jobtracker.plugins</name>\n')
            mapredsite.write('    <value>org.apache.hadoop.thriftfs.ThriftJobTrackerPlugin</value>\n')
            mapredsite.write('  </property>\n')
            mapredsite.write('</configuration>\n')
            put(mapredsite, 'mapred-site.xml')
        with TemporaryFile() as hdfssite:
            hdfssite.write('<?xml version="1.0">\n')
            hdfssite.write('<?xml-stylesheet type="text/xsl" href="configuration.xsl"?>\n')
            hdfssite.write('<configuration>\n')
            hdfssite.write('  <property>\n')        
            hdfssite.write('    <name>dfs.name.dir</name>\n')
            hdfssite.write('    <value>/data1/name</value>\n')
            hdfssite.write('  </property>\n')
            hdfssite.write('  <property>\n')        
            hdfssite.write('    <name>dfs.data.dir</name>\n')
            hdfssite.write('    <value>/data1/data</value>\n')
            hdfssite.write('  <!-- HUE integration -->\n')
            hdfssite.write('  <property>\n')        
            hdfssite.write('    <name>dfs.namenode.plugins</name>\n')
            hdfssite.write('    <value>org.apache.hadoop.thriftfs.NamenodePlugin</value>\n')
            hdfssite.write('  </property>\n')
            hdfssite.write('  <property>\n')        
            hdfssite.write('    <name>dfs.datanode.plugins</name>\n')
            hdfssite.write('    <value>org.apache.hadoop.thriftfs.DatanodePlugin</value>\n')
            hdfssite.write('  </property>\n')
            hdfssite.write('  <property>\n')        
            hdfssite.write('    <name>dfs.thrift.address</name>\n')
            hdfssite.write('    <value>0.0.0.0:10090</value>\n')
            hdfssite.write('  </property>\n')
            hdfssite.write('</configuration>\n')
            put(hdfssite, 'hdfs-site.xml')
    
def deploy_daemon(daemon):
    daemonPath = '/etc/init.d/hadoop-0.20-%s' % daemon
    if files.exists(daemonPath):
        run('%s stop' % daemonPath)
    run('yum -y install hadoop-0.20-%s' % daemon)
    run('%s start' % daemonPath)
    
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
