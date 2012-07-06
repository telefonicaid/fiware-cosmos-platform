"""
Cosmos automatic deployment Fabric file -

causes Fabric to deploy each Cosmos component at a configured host.
"""
import os
import json
from StringIO import StringIO

from fabric.api import run, execute, sudo, put, cd, env
from fabric.contrib import files
import fabric.context_managers as ctx
from fabric.colors import red, white, yellow
from fabric.decorators import roles, task, parallel
from fabric.utils import puts, error, warn
from mako.template import Template

import common
import iptables
import hadoop_install
import hue_deployment

BASEPATH = os.path.dirname(os.path.realpath(__file__))
CONFIG = json.loads(open(env.config, 'r').read())
env.roledefs = CONFIG['hosts']

@task
def deploy(dependenciespath, thrift_tar, jdk_rpm, move_sshd=False):
    """
    Deploys all the necessary components to get a running Cosmos cluster
    """
    report_current_task("open FTP ports configuration")
    execute(open_ftp_port)
    report_current_task("JDK")
    execute(deploy_jdk, os.path.join(dependenciespath, jdk_rpm))
    report_current_task("CDH")
    deploy_cdh()
    report_current_task("HUE")
    execute(deploy_hue, os.path.join(dependenciespath, thrift_tar))
    report_current_task("SFTP")
    execute(deploy_sftp, move_sshd)
    # report_current_task("Ganglia")
    # execute(deploy_ganglia)
    report_current_task("Mongo")
    execute(deploy_mongo)
    report_current_task("Provisioning users")
    execute(provision_user, "admin")

def report_current_task(task_name):
    report_start = "DEPLOY: starting %s deployment"
    puts(white(report_start % task_name, True))

@task
@roles('frontend')
def provision_user(user_name):
    run('su hdfs -c "hadoop dfs -mkdir /user/%s/{datasets,jars,tmp}"' %
            user_name)
    run('su hdfs -c "hadoop dfs -chown -R %s:hue /user/%s"' %
            (user_name, user_name))

@task
@parallel
@roles('namenode', 'frontend', 'jobtracker', 'mongo',
       'datanodes', 'tasktrackers')
def restore_iptables():
    """
    Debug function to revert to the default iptables in case something goes
    wrong
    """
    put(os.path.join(BASEPATH, 'templates/iptables'),
                     '/etc/sysconfig/iptables')
    sudo("service iptables restart")
    
@task
@parallel
@roles('namenode', 'frontend', 'jobtracker', 'mongo',
       'datanodes', 'tasktrackers')
def open_ftp_port():
    """
    Opening the FTP port is necessary in the staging cluster because  the
    internal yum repositories are FTP-based instead of the standard HTTP
    repos
    """
    iptables.accept_out_tcp(21)
    iptables.accept_out_tcp(22)
    sudo("service iptables save")

@task
@roles('frontend')
def add_test_setup():
    """
    Installs any test-specific setup components
    """
    files_to_delete = put(os.path.join(BASEPATH,
                          '../cosmos/tests/testUser.json'), 'testUser.json')
    with cd('/usr/share/hue'):
        run('build/env/bin/hue loaddata ~/testUser.json')
    for file_to_delete in files_to_delete:
        run('rm %s' % file_to_delete)
    execute(provision_user, 'test')

@task
@parallel
@roles('namenode', 'jobtracker', 'frontend', 'datanodes', 'tasktrackers')
def deploy_jdk(jdkpath):
    """
    Deployes basic packages that somehow are not present in Joyent templates.
    """
    if not common.has_package('wget'):
        run('yum -y install wget')
    if not common.has_package('sudo'):
        run('yum -y install sudo')
    if not common.has_package('jdk'):
        put(jdkpath, 'jdk.rpm')
        run('rpm -ihv jdk.rpm')

@task
@roles('frontend')
def deploy_hue(thrift_tarpath):
    """
    Deploys the HUE frontend from Cloudera, plus our fixes and our app
    """
    common.install_dependencies(['mysql-server', 'wget'])
    hue_deployment.install_git()
    hue_deployment.install_and_patch_hue(CONFIG)
    execute(hue_deployment.install_hue_plugins)
    hue_deployment.install_thrift(thrift_tarpath)
    hue_deployment.install_cosmos_app(CONFIG)
    hue_deployment.start_clean_database()
    hue_deployment.create_hue_tables()
    hue_deployment.create_admin_account()
    hue_deployment.start_daemons()
    hue_deployment.cleanup()

@task
@roles('frontend')
def deploy_sftp(move_sshd=False):
    """
    Deploys the SFTP server as a Java JAR and starts it
    """
    if isinstance(move_sshd, basestring) and\
            move_sshd in ['False', 'false', 'N', 'n']:
        move_sshd = False
    elif isinstance(move_sshd, basestring) and\
            move_sshd in ['True', 'true', 'Y', 'y']:
        move_sshd = True

    injection_exec = 'injection-server-{0}.jar'.format(CONFIG['version'])
    injection_jar = os.path.join(BASEPATH, CONFIG['injection_path'], 'target',
                                 injection_exec)
    if not os.path.exists(injection_jar):
        error(red('ERROR: injection server file not found: %s' %\
                  injection_jar))
        return
    exec_path = os.path.join('~', 'injection', injection_exec)
    if not files.exists(exec_path):
        run("mkdir -p {0}".format(os.path.join('~', 'injection')))
    put(injection_jar, exec_path)

    common.instantiate_template('templates/injection.conf.mako',
                                '/etc/injection.cfg', context=dict(
                                    hue_db_pwd=env.hue_db_pwd,
                                    namenode=CONFIG['hosts']['namenode'][0]
                                ))
    symlink = "/usr/local/injection-server"
    if not files.exists(symlink):
        run("ln -s {0} {1}".format(exec_path, symlink))
    common.touch_file("/var/log/injection/server.log")
    common.touch_file("/var/run/injection/server.pid")
    put(os.path.join(BASEPATH, "templates/injection.init.d"),
                               "/etc/init.d/injection")
    sudo("chmod +x /etc/init.d/injection")
    if move_sshd:
        custom_port = CONFIG['frontend_ssh_custom_port']
        warn(yellow("Moving sshd at %s to port %s" %
                    (env.host_string, custom_port)))
        do_move_sshd(custom_port)

    ## Fabric's pseudoterminal emulation does not play well with init scripts.
    ## All other components are safe to start within a pseudoterminal, but for
    ## the injection server it is safest to start from a real terminal.
    run("service injection restart", pty=False)

def do_move_sshd(custom_port=CONFIG['frontend_ssh_custom_port']):
    """
    Changes the port where the ssh daemon is listening. Note that after this
    change other tasks might fail because they try to login at the standard
    SSH port.
    """
    iptables.accept_in_tcp(custom_port)
    sudo("service iptables save")
    common.instantiate_template('templates/sshd_config.mako',
                                '/etc/ssh/sshd_config',
                                context=dict(new_port=custom_port))
    run("service sshd restart")

@task
def deploy_cdh():
    """Deploys the Cloudera Distribution for Hadoop"""
    execute(hadoop_install.install_cdh, CONFIG)
    execute(hadoop_install.create_hadoop_dirs, CONFIG)
    execute(hadoop_install.configure_hadoop, CONFIG)
    execute(hadoop_install.deploy_namenode_daemon)
    execute(hadoop_install.deploy_jobtracker_daemon)
    execute(hadoop_install.deploy_datanode_daemon)
    execute(hadoop_install.deploy_tasktracker_daemon)
  
@task
@roles('mongo')  
def deploy_mongo():
    """Install the latest MongoDB distribution"""
    iptables.accept_in_tcp(27017)
    iptables.accept_in_tcp(28017)
    with cd('/etc/yum.repos.d'):
        if not files.exists('10gen.repo'):
            put(os.path.join(BASEPATH, 'templates/10gen.repo'), '10gen.repo')
    run('yum -y install mongo-10gen mongo-10gen-server')
    common.instantiate_template('templates/mongod.conf.mako',
                                '/etc/mongod.conf', context=dict(
                                    dbpath=CONFIG['mongo_db_path']
                                ))
    run('service mongod start')
    run('chkconfig mongod on')
    run("mongo --eval \"db.adminCommand('listDatabases').databases.forEach("
        "function (d) { "
        "  if (d.name != 'local' && d.name != 'admin' && d.name != 'config')"
        "    db.getSiblingDB(d.name).dropDatabase();"
        "})\"")

@task
def deploy_ganglia():
    """
    Executes a number of steps in order to have Ganglia monitoring
    in the deployment cluster
    """
    execute(configure_hadoop_metrics)
    execute(configure_ntp)
    execute(install_gmetad)
    execute(install_ganglia_frontend)
    execute(install_gmond)

@parallel
@roles('namenode', 'namenode', 'jobtracker', 'frontend', 'datanodes', 'mongo',
       'tasktrackers')
def configure_ntp():
    """
    Configures -- and if not present, installs -- the NTP daemon. A coordinated
    time set in all hosts is necessary to have Ganglia monitoring.
    """
    common.install_dependencies(['ntp'])
    common.instantiate_template('templates/ntp.conf.mako', "/etc/ntp.conf")
    iptables.add_rule(("OUTPUT -p udp -d 0.rhel.pool.ntp.org "
                       "--dport 123 -j ACCEPT"))
    sudo("service iptables save")
    run("chkconfig --level 2 ntpd on")
    run("service ntpd start")

@roles('namenode')
def install_gmetad():
    """
    Installs the Ganglia daemon that collects monitoring information from many
    other Ganglia daemons
    """
    with ctx.hide('stdout'):
        repolist = run("yum repolist")
        epel_installed = any([line.split()[0] == 'epel'
                              for line in repolist.splitlines()])
        if not epel_installed:
            major_version = 6
            minor_version = 7
            repo_rpm = 'epel-release-{0}-{1}.noarch.rpm'.format(
                    major_version, minor_version)
            base_url = (('http://download.fedoraproject.org/pub/'
                         'epel/{0}/x86_64/'.format(major_version)))
            repo_url = base_url + repo_rpm
            run('wget %s' % repo_url)
            run('rpm -Uvh %s' % repo_rpm)
        run("yum -y erase ganglia-gmetad")
        run("yum -y install ganglia-gmetad")

    # FIXME: where does gmetad.conf go? /etc or /etc/ganglia
    gmetad_cfg_path = "/etc/gmetad.conf"
    if not files.exists(gmetad_cfg_path):
        run("mkdir -p /etc/ganglia")
        run("echo '' >> {0}".format(gmetad_cfg_path))
    common.instantiate_template('templates/gmetad.conf.mako', gmetad_cfg_path,
                                context=dict(
                                    monitored_hosts=common.clean_host_list(
                                        CONFIG['hosts'].values())))
    iptables.add_rule("INPUT -p tcp -d {0} --dport 8649 -j ACCEPT"
            .format(common.clean_host_list(CONFIG['hosts']['frontend'])))
    sudo("service iptables save")
    run("service gmetad start")
    run("chkconfig --level 2 gmetad on")

def install_epel():
    with ctx.hide('stdout'):
        repolist = run("yum repolist")
        epel_installed = any([line.split()[0] == 'epel'
                              for line in repolist.splitlines()])
        if not epel_installed:
            major_version = 6
            minor_version = 7
            repo_rpm = 'epel-release-{0}-{1}.noarch.rpm'.format(
                    major_version, minor_version)
            base_url = (('http://download.fedoraproject.org/pub/'
                         'epel/{0}/x86_64/'.format(major_version)))
            repo_url = base_url + repo_rpm
            run('wget %s' % repo_url)
            run('rpm -Uvh %s' % repo_rpm)

@roles('namenode')
def install_ganglia_frontend():
    """
    Installs the package that holds PHP scripts to query the gmetad daemon and
    present the results in graphical format. Also, starts an httpd to serve the
    resulting webpages.
    """
    with ctx.hide('stdout'):
        install_epel()
        run("yum -y erase ganglia-web")
        run("yum -y install ganglia-web")
    common.instantiate_template('templates/conf.php.mako', 
                                '/usr/share/ganglia/conf.php',
                                context=dict(gmetad_port = 8651))
    start = run("apachectl start")
    if start.endswith("already running"):
        run("apachectl restart")

@parallel
@roles('namenode', 'jobtracker', 'frontend', 'datanodes', 'mongo',
       'tasktrackers')
def install_gmond():
    """
    Installs the Ganglia daemon that collects information at each host in the
    cluster and should send this information to the gmetad.
    """
    with ctx.hide('stdout'):
        install_epel()
        run("yum -y erase ganglia-gmond")
        run("yum -y install ganglia-gmond")
    # FIXME: where gmond.conf goes? /etc or /etc/ganglia
    gmond_conf_path = "/etc/gmond.conf"
    if not files.exists(gmond_conf_path):
        run("mkdir -p /etc/ganglia")
        run("echo '' >> {0}".format(gmond_conf_path))
    gmetad_host = common.clean_host_list(CONFIG['hosts']['namenode'])
    common.instantiate_template('templates/gmond.conf.mako', gmond_conf_path,
                                context=dict(gmetad_host=gmetad_host))
    iptables.add_rule("OUTPUT -p udp -d {0} --dport 8649 -j ACCEPT"
            .format(common.clean_host_list(CONFIG['hosts']['frontend'])))
    sudo("service iptables save")
    run("service gmond start")
    run("chkconfig --level 2 gmond on")

@parallel
@roles('namenode', 'jobtracker', 'datanodes', 'tasktrackers')
def configure_hadoop_metrics():
    """
    Configures the Hadoop damons to send some metrics about their state to the
    gmond daemons
    """
    hadoop_version = CONFIG['hadoop_version']
    hadoop_metrics_path = "/etc/hadoop/conf/hadoop-metrics.properties"
    common.touch_file(hadoop_metrics_path)
    namenode = common.clean_host_list(CONFIG['hosts']['namenode'])
    common.instantiate_template('templates/hadoop-metrics.properties.mako',
                                hadoop_metrics_path,
                                context=dict(namenode=namenode))
    if files.exists("/etc/init.d/hadoop-%s-tasktracker" % hadoop_version):
        with ctx.hide('stdout'):
            run("service hadoop-%s-tasktracker restart" % hadoop_version)
