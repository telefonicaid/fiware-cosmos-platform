"""
common.py -

common functionality for Fabric deployments
"""
import os.path
from StringIO import StringIO

from fabric.api import env, put, roles, run, sudo, settings
from fabric.colors import green, white
from fabric.contrib import files
from fabric.decorators import roles
import fabric.context_managers as ctx
from mako.template import Template


BASEPATH = os.path.dirname(os.path.realpath(__file__))


def append_subdir_to_dirs(subdir_name, dirlist):
    return ','.join([os.path.join(directory, subdir_name) for directory in dirlist])

def install_cdh_repo(config):
    """Install the Hadoop distribution repo"""
    with ctx.hide('stdout'):
        cdh_repo = config["cdh_version_repo"].split("/")[-1]
        run('rm -rf /tmp/hadoop-*')
        run('wget %s' % config["cdh_version_repo"])
        run('rpm -Uvh --force %s' % cdh_repo)
        run('rm -f ' + cdh_repo)
        if not files.exists('/etc/pki/rpm-gpg/RPM-GPG-KEY-cloudera'):
            run(('rpm --import'
                 ' http://archive.cloudera.com/redhat/cdh/'
                 ' RPM-GPG-KEY-cloudera'))

def has_package(pkg):
    """
    Checks that a package is installed using the OS package manager
    """
    with ctx.hide('stdout'):
        with settings(warn_only=True):
            output = run('yum list -q installed | grep -qi %s' % pkg)
            if output.return_code != 0:
                print white("Package %s NOT installed on %s" %
                            (pkg , env.host_string), True)
                print output.stdout
                return 0
            else:
                print green("Package %s installed in %s" %
                            (pkg, env.host_string))
                return 1

def install_dependencies(pkg_list):
    """
    Checks that a list of dependencies is met using the OS package manager. If
    a dependency is not met, it is installed.
    """
    for pkg_name in pkg_list:
        if not has_package(pkg_name):
            run("yum -y install {0}".format(pkg_name))


def clean_host_list(hosts):
    """
    Perform all necessary operations to have a host list that can appear in
    monitoring configurations.
    """
    return ' '.join(set(remove_port_info(flatten(hosts))))


def flatten(nested_list):
    """
    Flattens a list that only contains strings or lists of strings, with only
    one level of depth.
    """
    ans = []
    for elem in nested_list:
        if isinstance(elem, basestring):
            ans.append(elem)
        else:
            ans.extend(elem)
    return ans


def remove_port_info(hosts):
    """
    Remove port informations from a list of host IPs
    """
    ans = []
    for host in hosts:
        if len(host.split(':')) > 1:
            ans.append(host.split(':')[0])
        else:
            ans.append(host)
    return ans


def instantiate_template(template_file, output_file, context={},
                         basepath=BASEPATH):
    """
    Instantiate a mako template on a file on the destination host
    (fabric.api.put).
    """
    template = Template(filename = os.path.join(basepath, template_file))
    content = StringIO()
    content.write(template.render(**context))
    put(content, output_file)


def touch_file(file_path):
    """
    Makes sure a file exists creating an empty one if it does not exists.
    """
    if not files.exists(file_path):
        dir_path = os.path.dirname(file_path)
        run("mkdir -p %s" % dir_path)
        run("echo '' >> {0}".format(file_path))
