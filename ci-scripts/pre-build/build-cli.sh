#!/bin/bash
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
set -e

cd $WORKSPACE/cosmos-cli
python2.7 ./bootstrap.py
if [ -f jenkins.cfg ]; then
  bin/buildout -c jenkins.cfg
  bin/jenkins-test
  set +e
  bin/jenkins-test-coverage
  bin/jenkins-code-analysis-pep8
  bin/jenkins-code-analysis-pyflakes
  bin/jenkins-code-analysis-flake8
  bin/jenkins-code-analysis-utf8header
else
  bin/buildout
  bin/test
  set +e
fi
bin/buildout setup . bdist_egg
