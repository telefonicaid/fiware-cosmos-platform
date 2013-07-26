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

setup(name='cosmos',
      version='0.1',
      author='Cosmos Team',
      author_email='cosmos@tid.es',
      packages=['cosmos'],
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
