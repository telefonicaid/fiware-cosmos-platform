"""
common.py -

common functionality for Fabric deployments
"""
from fabric.api import env, roles, run, settings
from fabric.colors import red, green
from fabric.decorators import roles
import fabric.context_managers as ctx

@roles('namenode')
def has_package(pkg):
    """
    Checks that a package is installed using the OS package manager
    """
    with ctx.hide('stdout'):
        # Need to consider this settings context and warn_only
        # to have full control over the run commands.
        with settings(warn_only=True):
            output=run('yum list -q installed |grep -qi %s' % pkg)
            if output.return_code != 0:
                print red("ERROR: Package %s NOT installed on %s" % (pkg , env.host_string))
                print output.stdout
                return 0
            else:
                print green("Package %s installed in %s" % (pkg, env.host_string))
                return 1

def has_package2(pkg):
    """
    Checks that a package is installed using the OS package manager
    """
    with ctx.hide('stdout'):
        out = run('yum list -q installed | grep ^%s\\. || echo' % pkg)
        return len(out.strip()) > 0

def install_dependencies(pkg_list):
    """
    Checks that a list of dependencies is met using the OS package manager. If
    a dependency is not met, it is installed.
    """
    for pkg_name in pkg_list:
        if not has_package(pkg_name):
            run("yum -y install {}".format(pkg_name))

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
    ## TODO: review in light of: all roles are lists
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
