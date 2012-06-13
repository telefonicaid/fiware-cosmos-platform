# -*- coding: utf-8 -*-
"""HUE configuration script"""

import os
import os.path
import re
from shutil import copy, copytree
import subprocess
import sys


def virtualenv_safe_environment(env):
    safe_env = env.copy()
    safe_env.pop('PYTHONPATH', '')
    safe_env.pop('BUILDOUT_ORIGINAL_PYTHONPATH', '')
    return safe_env


def run_subprocess(command_parts, description=None, environ=None):
    if description is None:
        description = "'%s'" % " ".join(command_parts)
    if environ is None:
        environ = os.environ
    print description
    retval = subprocess.call(command_parts,
                             env=virtualenv_safe_environment(environ))
    if retval != 0:
        raise RuntimeError("Error %d while %s" % (retval, description))


def pip_install(hue_path, package):
    run_subprocess([os.path.join(hue_path, 'build/env/bin/pip'), 'install',
                    package], description="Installing package %s" % package)


def make(hue_path, target):
    environ = os.environ.copy()
    environ['SYS_PYTHON'] = sys.executable
    environ['SKIP_PYTHONDEV_CHECK'] = '1'
    run_subprocess(['make', '-C', hue_path, target], environ=environ)


def install_app(hue_path, app_path):
    run_subprocess([os.path.join(hue_path, 'build/env/bin/python'),
                    os.path.join(hue_path, 'tools/app_reg/app_reg.py'),
                    '--install', app_path],
                   description="Installing app %s" % app_path)


def link_hue_script(hue_path, bin_path):
    print "Linking hue script"
    dest_path = os.path.join(bin_path, 'hue')
    if os.path.exists(dest_path):
        os.remove(dest_path)
    os.symlink(os.path.join(hue_path, "build/env/bin/hue"), dest_path)


def configure_env(options, buildout):
    hue_path = options['hue']
    make(hue_path, 'virtual-env')
    for package in ['MySQL-python', 'pymongo', 'lxml', 'nose', 'NoseXUnit',
                    'windmill', 'pylint', 'ipdb']:
        pip_install(hue_path, package)
    make(hue_path, 'apps')
    install_app(hue_path, os.path.abspath(buildout['buildout']['develop']))
    link_hue_script(hue_path, buildout['buildout']['bin-directory'])


def copy_app_sources(app_path, buildout_path):
    if not os.path.exists(app_path):
        os.mkdir(app_path)
    for f in ['src', 'conf', 'Makefile', 'setup.py']:
        src = os.path.join(buildout_path, f)
        if os.path.isdir(src):
            copytree(src, os.path.join(app_path, f))
        else:
            copy(src, app_path)


def deploy_app(options, buildout):
    hue_path = options['hue']
    pip_install(hue_path, 'pymongo')
    app_path = os.path.join(hue_path, 'apps/cosmos/')
    buildout_path = os.path.abspath(buildout['buildout']['develop'])
    copy_app_sources(app_path, buildout_path)
    install_app(hue_path, app_path)
