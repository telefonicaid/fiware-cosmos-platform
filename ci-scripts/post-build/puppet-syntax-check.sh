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
echo "==== CHECKING PUPPET SYNTAX ===="
ALL_PUPPET_FILES=$(find $WORKSPACE/deployment/puppet -iname '*.pp')
puppet parser validate --color false --render-as s \
     $ALL_PUPPET_FILES || exit 1

echo "==== CHECKING PUPPET STYLE ===="
find $WORKSPACE/deployment/puppet/modules -iname '*.pp' -exec puppet-lint \
    --no-class_inherits_from_params_class-check --no-80chars-check \
    --no-class_parameter_defaults-check \
    --log-format "%{path}:%{linenumber}:%{check}:%{KIND}:%{message}" {} \;
