'''
Created on 01/03/2012

@author: def
'''
from sys import argv

got_devtools = True
try:
    from devTools import setup as pssetup
    from devTools import getRevision
except ImportError, e:
    print "ERROR:",e
    got_devtools = False

got_setuptools = True
try:
    from setuptools import setup as toolssetup
    from setuptools import find_packages
    from setuptools import join
except ImportError, e:
    print "ERROR:",e
    got_setuptools = False

if not "bdist_rpm" in argv or "--help" in argv:
    if not got_setuptools:
        print "ERROR: Python setuptools required for this operation"
        exit(1);
    setup = toolssetup
else:
    if not got_devtools:
        print "ERROR: PS MongoDB devTools required for this operation"
        exit(1);
    setup = pssetup

setup(
    name = "bdp-monitoring",
    version = "0.1",
    description = "Big Data Platform Monitoring",
    long_description = "Big Data Platform Monitoring",
    author = "Telefonica R&D",
    url = "http://tid.es/en",
    packages = ['Configuration'] , #find_packages(exclude=['test*','wizard','templates']),
    data_files=[
        ('/etc/httpd/conf.d', ['apacheConf/monitoring.conf']),
        ('/usr/local/ps/apps/bdp/Configuration', ['Configuration/settings_prod.py'])],
        options = {'bdist_rpm':{'release' : getRevision(),
                            'requires' : 'httpd', #mod_wsgi, django-nonrel, pymongo, djangotoolbox, django-mongodb',
                            'post_install' : 'scripts/postInstall.sh',
                            'pre_install' : 'scripts/preInstall.sh',
                            #'pre_uninstall' : 'scripts/preUninstall.sh',
                            'post_uninstall' : 'scripts/postUninstall.sh',
                            }},
    )