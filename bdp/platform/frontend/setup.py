"""
Created on 21/03/2012

@author: losa, sortega
"""

import os

from setuptools import setup, find_packages


def find_files(path):
    files = []
    for dirname, subdirnames, filenames in os.walk(path):
        for subdirname in subdirnames:
            files.extend(find_files(os.path.join(dirname, subdirname)))
        for filename in filenames:
            files.append(os.path.join(dirname, filename))
    return files

setup(
    name = "bdp_fe",
    version = "0.1.0",
    description = "Big Data Platform Frontend",
    long_description = ("This package is a web interface for the Big Data "
                        "Platform.  Through this frontend, a user can lauch "
                        "Haddop jobs, read and interpret its results."),
    author = "Telefonica Digital",
    author_email = "cosmos@tid.es",
    package_dir = {'': 'src'},
    packages = find_packages('src'),
    package_data = {'': ['templates/*']},
    data_files = [('share/bdp_fe/static', 
                   find_files('src/bdp_fe/jobconf/static/'))],
    install_requires = [
        'setuptools',
        'pymongo',
        'django',
        'coverage',
        'django-jenkins',
        'thrift',
        'flup',
        'MySQL-python',
        ],
    classifiers = [
        "Development Status :: 3 - Alpha",
    ],
)
