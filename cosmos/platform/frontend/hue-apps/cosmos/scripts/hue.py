# -*- coding: utf-8 -*-
"""HUE configuration script"""

import os
import os.path
import re
from shutil import copy, copytree
import subprocess


def pip_install(hue_path, package):
    print "Installing package %s" % package
    subprocess.check_call([os.path.join(hue_path, 'build/env/bin/pip'),
                           'install', package])


def make(hue, target):
    print "make %s" % target
    subprocess.check_call(['make', '-C', hue, target])


def install_app(hue_path, app_path):
    print "Installing app %s" % app_path
    subprocess.check_call([os.path.join(hue_path, 'build/env/bin/python'), 
                           os.path.join(hue_path, 'tools/app_reg/app_reg.py'),
                           '--install', app_path])


def link_hue_script(hue_path, bin_path):
    print "Linking hue script"
    dest_path = os.path.join(bin_path, 'hue')
    if os.path.exists(dest_path):
        os.remove(dest_path)
    os.symlink(os.path.join(hue_path, "build/env/bin/hue"), dest_path)


def configure_env(options, buildout):
    hue = options['hue']
    make(hue, 'virtual-env')
    for package in ['MySQL-python', 'pymongo', 'lxml', 'ipdb',
                    'werkzeug', 'nose', 'NoseXUnit', 'windmill']:
        pip_install(hue, package)
    make(hue, 'apps')
    install_app(hue, os.path.abspath(buildout['buildout']['develop']))
    link_hue_script(hue, buildout['buildout']['bin-directory'])


def deploy_app(options, buildout):
    hue_path = options['hue']
    buildout_path = os.path.abspath(buildout['buildout']['develop'])
    app_path = os.path.join(hue_path, 'apps/cosmos/')
    if not os.path.exists(app_path):
        os.mkdir(app_path)
    for f in ['src', 'conf', 'Makefile', 'setup.py']:
        src = os.path.join(buildout_path, f)
        if os.path.isdir(src):
            copytree(src, os.path.join(app_path, f))
        else:
            copy(src, app_path)
    install_app(hue_path, app_path)
