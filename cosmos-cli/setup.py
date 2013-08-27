# -*- coding: utf-8 -*-
#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#
from setuptools import setup

from version import parse_pom_version


SNAPSHOT_SUFFIX='-SNAPSHOT'


def lookup_version():
    pom_version = parse_pom_version('../pom.xml')
    if pom_version.endswith(SNAPSHOT_SUFFIX):
        # EGG timestamp is managed at deployment time so we remove the
        # snapshot suffix here
        return pom_version[:-len(SNAPSHOT_SUFFIX)]
    else:
        return pom_version


setup(name='cosmos',
      version=lookup_version(),
      author='Cosmos Team',
      author_email='cosmos@tid.es',
      packages=['cosmos', 'cosmos.cli', 'cosmos.common', 'cosmos.compute', 'cosmos.storage'],
      entry_points={
          'console_scripts': ['cosmos=cosmos.cli.main:run']},
      install_requires=[
          'pyyaml',
          'pymlconf',
          'requests',
      ],
      extras_require=dict(
          test=[
              'mock',
              'web.py',
              'testfixtures'
          ])
     )
