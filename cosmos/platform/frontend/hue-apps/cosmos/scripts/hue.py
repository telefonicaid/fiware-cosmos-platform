# -*- coding: utf-8 -*-
"""HUE configuration script"""

import os
import os.path
import re
import subprocess


def pip_install(hue, package):
    print "Installing package %s" % package
    subprocess.check_call(['%s/build/env/bin/pip' % hue, 'install', package])


def make(hue, target):
    print "make %s" % target
    subprocess.check_call(['make', '-C', hue, target])


def install_app(hue_path, app_path):
    print "Installing app %s" % app_path
    subprocess.check_call(['%s/build/env/bin/python' % hue_path, 
                           '%s/tools/app_reg/app_reg.py' % hue_path,
                           '--install', app_path])


def link_hue_script(hue_path, bin_path):
    print "Linking hue script"
    dest_path = "%s/hue" % bin_path
    if os.path.exists(dest_path):
        os.remove(dest_path)
    os.symlink("%s/build/env/bin/hue" % hue_path, dest_path)


def configure_env(options, buildout):
    hue = options['hue']
    make(hue, 'virtual-env')
    for package in ['MySQL-python', 'pymongo', 'lxml', 'werkzeug', 'nose',
                    'windmill', 'django-jenkins', 'coverage', 'pep8', 'pylint']:
        pip_install(hue, package)
    make(hue, 'apps')
    install_app(hue, os.path.abspath(buildout['buildout']['develop']))
    link_hue_script(hue, buildout['buildout']['bin-directory'])
