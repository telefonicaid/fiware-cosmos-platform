"""
common.py -

common functionality for Fabric deployments
"""
import fabric.context_managers as ctx
from fabric.api import run

def has_package(pkg):
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
