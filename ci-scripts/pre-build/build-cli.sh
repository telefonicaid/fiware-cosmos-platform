#!/bin/bash
#
# Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
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
