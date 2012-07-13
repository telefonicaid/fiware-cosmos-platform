"""
Hadoop automatic deployment
"""
import os.path

from fabric.api import cd, env, put, run, sudo
import fabric.context_managers as ctx
from fabric.contrib import files
from fabric.colors import green, red
from fabric.decorators import roles, parallel
from fabric.utils import puts
from StringIO import StringIO
from mako.template import Template

import common
import iptables
from sockpuppet import SockPuppet

COSMOS_CLASSPATH = '/usr/lib/hadoop-0.20/lib/cosmos/'
HADOOP_GPL_CP = '/opt/hadoopgpl'
BASEPATH = os.path.dirname(os.path.realpath(__file__))

@parallel
@roles('namenode', 'jobtracker', 'datanodes', 'tasktrackers')
def install_cdh(config):
    """Install the latest Hadoop distribution in CDH3"""
    common.install_cdh_repo(config)
    with ctx.hide('stdout'):
        run('yum -y install hadoop-0.20 hadoop-0.20-native')


@parallel
@roles('namenode', 'jobtracker', 'datanodes', 'tasktrackers')
def create_hadoop_dirs(config):
    """Create necessary directories for Hadoop"""
    for conf_dir in config["hadoop_data_dirs"]:
        run('rm -rf %s/*' % conf_dir)
        if not files.exists(conf_dir):
            run('mkdir %s' % conf_dir)
        run('install -o hdfs   -g hadoop -m 755 -d %s/data' % conf_dir)
        run('install -o mapred -g hadoop -m 755 -d %s/mapred' % conf_dir)
        run('install -o hdfs   -g hadoop -m 755 -d %s/name' % conf_dir)

    run('install -o root   -g hadoop -m 755 -d %s' % COSMOS_CLASSPATH)

@parallel
@roles('namenode', 'jobtracker', 'datanodes', 'tasktrackers')
def hadoopgpl_compression(hadoopgpl_rpm):
    sockpuppet = SockPuppet()
    local_path = hadoopgpl_rpm
    rpm_name = os.path.split(hadoopgpl_rpm)[-1]
    remote_path = 'hadoopgpl-compression/'
    sockpuppet.upload_file(local_path, remote_path)
    with cd(sockpuppet.get_remote_tempdir()):
        run('yum localinstall -y --nogpgcheck %s' % rpm_name)
    sockpuppet.cleanup_uploaded_files()

@parallel
@roles('namenode', 'jobtracker', 'datanodes', 'tasktrackers', 'frontend')
def configure_hadoop(config):
    """Generate  Hadoop configuration files"""
    with cd('/etc/hadoop/conf'):
        common.instantiate_template('templates/core-site.mako',
                                    'core-site.xml', context=dict(
                                        namenode=config['hosts']['namenode'][0]
                                    ))
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

        max_reducers = 2*len(config['hosts']['datanodes'])
        common.instantiate_template(
            'templates/mapred-site.mako',
            'mapred-site.xml',
            context={
                    'jobtracker': config['hosts']['jobtracker'][0],
                    'dirs': common.append_subdir_to_dirs('mapred',
                                                config['hadoop_data_dirs']),
                    'max_reduce_tasks': max_reducers,
                    'reduce_tasks': int(0.9 * max_reducers),
                    'hadoopgpl_native_libs': os.path.join(HADOOP_GPL_CP,
                                                   '/native/Linux-amd64-64')
                    }
            )

        common.instantiate_template(
            'templates/hdfs-site.mako',
            'hdfs-site.xml',
            context={
                'namedirs': common.append_subdir_to_dirs('name',
                                                config['hadoop_data_dirs']),
                'datadirs': common.append_subdir_to_dirs('data',
                                                config['hadoop_data_dirs']),
                'namenode': config['hosts']['namenode'][0]
                }
            )

        common.instantiate_template(
            'templates/hadoop-env.mako',
            'hadoop-env.sh',
            context={
                'cosmos_classpath': COSMOS_CLASSPATH,
                'hadoopgpl_lzo_lib': os.path.join(HADOOP_GPL_CP,
                                                  '/lib/hadoop-lzo.jar')
                }
            )

def deploy_daemon(daemon):
    """Deploys a Hadoop daemon"""
    daemon_path = '/etc/init.d/hadoop-0.20-%s' % daemon
    if files.exists(daemon_path):
        run('%s stop' % daemon_path)
    run('yum -y install hadoop-0.20-%s' % daemon)

def start_daemon(daemon):
    """Starts a Hadoop daemon"""
    run('/etc/init.d/hadoop-0.20-%s start' % daemon, pty=False)

@parallel
@roles('datanodes')
def deploy_datanode_daemon():
    """Deploys the datanode Hadoop daemon"""
    iptables.accept_in_tcp(50010)
    iptables.accept_in_tcp(1004)
    iptables.accept_in_tcp(50075)
    iptables.accept_in_tcp(1006)
    iptables.accept_in_tcp(50020)
    iptables.accept_in_tcp(10091)
    iptables.accept_in_tcp(0)
    sudo("service iptables save")
    deploy_daemon('datanode')
    start_daemon('datanode')

@roles('namenode')
def deploy_namenode_daemon():
    """Deploys the namenode Hadoop daemon"""
    iptables.accept_in_tcp(8020)
    iptables.accept_in_tcp(50070)
    iptables.accept_in_tcp(50470)
    iptables.accept_in_tcp(10090)
    iptables.accept_in_tcp(50090)
    iptables.accept_in_tcp(50495)
    sudo("service iptables save")
    deploy_daemon('namenode')
    output = sudo('yes Y | hadoop namenode -format', user='hdfs')
    if output.return_code != 0:
        red('Error while deploying namenode daemon on %s' % env.host)
        puts(output.stdout)
    else:
        green('Success deploying namenode daemon on %s' % env.host)
    start_daemon('namenode')
    sudo('hadoop dfs -mkdir /hadoop/mapred/system', user='hdfs')
    sudo('hadoop dfs -chown -R mapred /hadoop/mapred/', user='hdfs')

@roles('jobtracker')
def deploy_jobtracker_daemon():
    """Deploys the jobtracker Hadoop daemon"""
    iptables.add_rule("INPUT -p tcp --dport 8021 -j ACCEPT")
    iptables.add_rule("INPUT -p tcp --dport 50030 -j ACCEPT")
    iptables.add_rule("INPUT -p tcp --dport 9290 -j ACCEPT")
    sudo("service iptables save")
    deploy_daemon('jobtracker')
    start_daemon('jobtracker')

@parallel
@roles('tasktrackers')
def deploy_tasktracker_daemon():
    """Deploys the tasktracker Hadoop daemon"""
    iptables.accept_in_tcp(50060)
    iptables.accept_in_tcp(0)
    sudo("service iptables save")
    deploy_daemon('tasktracker')
    start_daemon('tasktracker')
