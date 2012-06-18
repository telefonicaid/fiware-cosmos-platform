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
from fabric.colors import green, red
from fabric.decorators import roles, task, parallel
from fabric.utils import puts
from mako.template import Template

import common
import hadoop_install
import hue_deployment

CONFIG = json.loads(open(env.config, 'r').read())
env.roledefs = CONFIG['hosts']
BASEPATH = os.path.dirname(os.path.realpath(__file__))

@task
def deploy(dependenciespath, thrift_tar, jdk_rpm):
    """
    Deploys all the necessary components to get a running Cosmos cluster
    """
    execute(deploy_jdk, os.path.join(dependenciespath, jdk_rpm))
    deploy_cdh()
    execute(deploy_hue, os.path.join(dependenciespath, thrift_tar))
    execute(deploy_sftp)
    execute(deploy_ganglia)
    execute(deploy_mongo)

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
    common.install_dependencies(['mysql', 'git'])
    hue_deployment.install_and_patch_hue(CONFIG)
    execute(hue_deployment.install_hue_plugins)
    hue_deployment.install_thrift(thrift_tarpath)
    hue_deployment.install_cosmos_app()
    hue_deployment.start_daemons()
    hue_deployment.cleanup()

@task
@roles('frontend')
def deploy_sftp():
    """
    Deploys the SFTP server as a Java JAR and starts it
    """
    injection_exec = 'injection-server-{0}.jar'.format(CONFIG['version'])
    injection_jar = os.path.join(BASEPATH, CONFIG['injection_path'], 'target',
                                 injection_exec)
    exec_path = os.path.join('~', 'injection', injection_exec)

    put(injection_jar, exec_path)

    injection_conf = StringIO()
    template = Template(filename = os.path.join(BASEPATH,'templates/injection.conf.mako'))
    injection_conf.write(template.render(
            namenode=CONFIG['hosts']['namenode'][0]))
    put(injection_conf, "/etc/injection.cfg")
    symlink = "/usr/local/injection-server"
    if not files.exists(symlink):
        run("ln -s {0} {1}".format(exec_path, symlink))
    logfile = "/var/log/injection/server.log"
    if not files.exists(logfile):
        run("mkdir -p /var/log/injection")
        run("echo '' >> {0}".format(logfile))
    pidfile = "/var/run/injection/server.pid"
    if not files.exists(pidfile):
        run("mkdir -p /var/run/injection")
        run("echo '' >> {0}".format(pidfile))
    put(os.path.join(BASEPATH, "templates/injection.init.d"), "/etc/init.d/injection")
    with ctx.settings(warn_only=True):
        start = run("/etc/init.d/injection start", pty=False)
        if start.failed:
            run("/etc/init.d/injection restart", pty=False)

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
    with cd('/etc/yum.repos.d'):
        if not files.exists('10gen.repo'):
            put(os.path.join(BASEPATH, 'templates/10gen.repo'), '10gen.repo')
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
    ## TODO: define in configuration
    model_paths = []
    for model in model_paths:
        put(model)
        sudo('hadoop dfs -put {0} /models/{0}'.format(model))
        run('rm %s' % model)

@task
def deploy_ganglia():
    execute(configure_hadoop_metrics)
    execute(configure_ntp)
    execute(install_gmetad)
    execute(install_ganglia_frontend)
    execute(install_gmond)

@parallel
@roles('namenode', 'namenode', 'jobtracker', 'frontend', 'datanodes', 'mongo',
       'tasktrackers')
def configure_ntp():
    ntp_conf = StringIO()
    template = Template(filename='templates/ntp.conf.mako')
    content = template.render()
    ntp_conf.write(content)
    ntp_conf_path = "/etc/ntp.conf"
    put(ntp_conf, ntp_conf_path)
    run("chkconfig --level 2 ntpd on")
    run("service ntpd start")

@roles('namenode')
def install_gmetad():
    with ctx.hide('stdout'):
        repolist = run("yum repolist")
        epel_installed = any([line.split()[0] == 'epel'
                              for line in repolist.splitlines()])
        if not epel_installed:
            major_version = 5
            minor_version = 4
            repo_rpm = 'epel-release-{0}-{1}.noarch.rpm'.format(
                    major_version, minor_version)
            base_url = ('http://download.fedoraproject.org/pub/epel/{0}/i386/'
                            .format(major_version))
            repo_url = base_url + repo_rpm
            run('wget %s' % repo_url)
            run('rpm -Uvh %s' % repo_rpm)
        run("yum -y erase ganglia-gmetad")
        run("yum -y install ganglia-gmetad")
    gmetad_conf = StringIO()
    template = Template(filename='templates/gmetad.conf.mako')
    content = template.render(
            monitored_hosts = common.clean_host_list(CONFIG['hosts'].values()))
    gmetad_conf.write(content)
    gmetad_cfg_path = "/etc/gmetad.conf"
    if not files.exists(gmetad_cfg_path):
        run("mkdir -p /etc/ganglia")
        run("echo '' >> {0}".format(gmetad_cfg_path))
    put(gmetad_conf, gmetad_cfg_path)
    run("service gmetad start")
    run("chkconfig --level 2 gmetad on")

@roles('namenode')
def install_ganglia_frontend():
    with ctx.hide('stdout'):
        repolist = run("yum repolist")
        epel_installed = any([line.split()[0] == 'epel'
                              for line in repolist.splitlines()])
        if not epel_installed:
            major_version = 5
            minor_version = 4
            repo_rpm = 'epel-release-{0}-{1}.noarch.rpm'.format(
                    major_version, minor_version)
            base_url = ('http://download.fedoraproject.org/pub/epel/{0}/i386/'
                            .format(major_version))
            repo_url = base_url + repo_rpm
            run('wget %s' % repo_url)
            run('rpm -Uvh %s' % repo_rpm)
        run("yum -y erase ganglia-web")
        run("yum -y install ganglia-web")
    ganglia_web_conf = StringIO()
    template = Template(filename='templates/conf.php.mako')
    content = template.render(gmetad_port = 8651)
    ganglia_web_conf.write(content)
    ganglia_web_conf_path = "/usr/share/ganglia/conf.php"
    put(ganglia_web_conf, ganglia_web_conf_path)
    start = run("apachectl start")
    if start.endswith("already running"):
        run("apachectl restart")

@parallel
@roles('namenode', 'jobtracker', 'frontend', 'datanodes', 'mongo',
       'tasktrackers')
def install_gmond():
    with ctx.hide('stdout'):
        repolist = run("yum repolist")
        epel_installed = any([line.split()[0] == 'epel'
                              for line in repolist.splitlines()])
        if not epel_installed:
            major_version = 5
            minor_version = 4
            repo_rpm = 'epel-release-{0}-{1}.noarch.rpm'.format(
                    major_version, minor_version)
            base_url = ('http://download.fedoraproject.org/pub/epel/{0}/i386/'
                            .format(major_version))
            repo_url = base_url + repo_rpm
            run('wget %s' % repo_url)
            run('rpm -Uvh %s' % repo_rpm)
        run("yum -y erase ganglia-gmond")
        run("yum -y install ganglia-gmond")
    gmond_conf = StringIO()
    template = Template(filename='templates/gmond.conf.mako')
    content = template.render(
            gmetad_host = common.clean_host_list(CONFIG['hosts']['namenode']))
    gmond_conf.write(content)
    gmond_conf_path = "/etc/gmond.conf"
    if not files.exists(gmond_conf_path):
        run("mkdir -p /etc/ganglia")
        run("echo '' >> {0}".format(gmond_conf_path))
    put(gmond_conf, gmond_conf_path)
    run("service gmond start")
    run("chkconfig --level 2 gmond on")

@parallel
@roles('namenode', 'jobtracker', 'datanodes', 'tasktrackers')
def configure_hadoop_metrics():
    hadoop_metrics_conf = StringIO()
    template = Template(filename='templates/hadoop-metrics.properties.mako')
    content = template.render(
            namenode = common.clean_host_list(CONFIG['hosts']['namenode']))
    hadoop_metrics_conf.write(content)
    ## TODO: parametize Hadoop version
    hadoop_metrics_path = "/etc/hadoop-0.20/conf/hadoop-metrics.properties"
    if not files.exists(hadoop_metrics_path):
        run("mkdir -p /etc/hadoop-0.20/conf/")
        run("echo '' >> {0}".format(hadoop_metrics_path))
    put(hadoop_metrics_conf, hadoop_metrics_path)
    if files.exists("/etc/init.d/hadoop-0.20-tasktracker"):
        with ctx.hide('stdout'):
            run("service hadoop-0.20-tasktracker restart")
