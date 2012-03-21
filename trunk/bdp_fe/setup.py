"""
Created on 21/03/2012

@author: losa
"""

from setuptools import setup, find_packages

setup(
    name = "bdp_fe",
    version = "0.1",
    description = "Big Data Platform Frontend",
    long_description = open("README.rst", "r").read(),
    author = "Telefonica Digital",
    package_dir = {'': 'src'},
    packages = ['bdp_fe', 'bdp_fe.jobconf'], #find_packages('src'),
    install_requires = [
        'setuptools',
        'pymongo',
        #'django',
        ],
    entry_points = {
        #'console_scripts': [
            #'instance = bdp-fe:run',
            #]
        },
    )
