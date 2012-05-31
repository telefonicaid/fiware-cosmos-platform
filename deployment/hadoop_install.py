"""
Hadoop automatic deployment
"""
from fabric.api import run, sudo, put, cd
from fabric.decorators import roles, parallel
from fabric.contrib import files
from StringIO import StringIO
from mako.template import Template

COSMOS_CLASSPATH='/usr/lib/hadoop-0.20/lib/cosmos/'

@roles('namenode', 'jobtracker', 'datanodes', 'tasktrackers')
@parallel
def install_cdh():
    """Install the latest Hadoop distribution in CDH3"""
    repo_rpm = ('http://archive.cloudera.com/redhat/cdh/'
                'cdh3-repository-1.0-1.noarch.rpm')
    run('wget %s' % repo_rpm)
    run('rpm -Uvh --force cdh3-repository-1.0-1.noarch.rpm')
    run(('rpm --import'
         ' http://archive.cloudera.com/redhat/cdh/RPM-GPG-KEY-cloudera'))
    run('yum -y install hadoop-0.20 hadoop-0.20-native')


@roles('namenode', 'jobtracker', 'datanodes', 'tasktrackers')
@parallel
def create_hadoop_dirs():
    """Create necessary directories for Hadoop"""
    run('rm -rf /data1')
    run('install -o hdfs   -g hadoop -m 700 -d /data1/data')
    run('install -o mapred -g hadoop -m 755 -d /data1/mapred')
    run('install -o hdfs   -g hadoop -m 755 -d /data1/name')
    run('install -o root   -g hadoop -m 755 -d %s' % COSMOS_CLASSPATH)
 
@roles('namenode', 'jobtracker', 'datanodes', 'tasktrackers')
@parallel
def configure_hadoop(config):
    """Generate  Hadoop configuration files"""
    with cd('/etc/hadoop/conf'):
        coresite = StringIO()
        template = Template(filename='templates/core-site.mako')
        coresite.write(template.render(
                namenode=config['hosts']['namenode'][0]))
        put(coresite, 'core-site.xml')
        
        masters = StringIO()
        for master in set(config['hosts']['namenode'] +\
                          config['hosts']['jobtracker']):
            masters.write('%s\n' % master)
        put(masters, 'masters')
        
        slaves = StringIO()
        for slave in set(config['hosts']['datanodes'] +\
                         config['hosts']['tasktrackers']):
            slaves.write('%s\n' % slave)
        put(slaves, 'slaves')
            
        mapredsite = StringIO()
        template = Template(filename='templates/mapred-site.mako')
        mapredsite.write(template.render(
                jobtracker=config['hosts']['jobtracker'][0]))
        put(mapredsite, 'mapred-site.xml')
        
        hdfssite = StringIO()
        template = Template(filename='templates/hdfs-site.mako')
        hdfssite.write(template.render())
        put(hdfssite, 'hdfs-site.xml')

        hadoop_env = StringIO()
        template = Template(filename='templates/hadoop-env.mako')
        hadoop_env.write(template.render(
            cosmos_classpath=COSMOS_CLASSPATH))
        put(hadoop_env, 'hadoop-env.sh')
           
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
@parallel
def deploy_datanode_daemon():
    """Deploys the datanode Hadoop daemon"""
    deploy_daemon('datanode')
    start_daemon('datanode')

@roles('namenode')
@parallel
def deploy_namenode_daemon():
    """Deploys the namenode Hadoop daemon"""
    deploy_daemon('namenode')
    sudo('yes Y | hadoop namenode -format', user='hdfs')
    start_daemon('namenode')
    sudo('hadoop dfs -mkdir /hadoop/mapred/system', user='hdfs')
    sudo('hadoop dfs -chown -R mapred /hadoop/mapred/', user='hdfs')

@roles('jobtracker')
@parallel
def deploy_jobtracker_daemon():
    """Deploys the jobtracker Hadoop daemon"""
    deploy_daemon('jobtracker')
    start_daemon('jobtracker')

@roles('tasktrackers')   
@parallel 
def deploy_tasktracker_daemon():
    """Deploys the tasktracker Hadoop daemon"""
    deploy_daemon('tasktracker')
    start_daemon('tasktracker')
