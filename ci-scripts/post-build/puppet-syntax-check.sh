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
echo "==== CHECKING PUPPET SYNTAX ===="
ALL_PUPPET_FILES=$(find $WORKSPACE/deployment/puppet -iname '*.pp')
puppet parser validate --color false --render-as s \
     $ALL_PUPPET_FILES || exit 1

echo "==== CHECKING PUPPET STYLE ===="
find $WORKSPACE/deployment/puppet/modules -iname '*.pp' -exec puppet-lint \
    --no-class_inherits_from_params_class-check --no-80chars-check \
    --no-class_parameter_defaults-check \
    --log-format "%{path}:%{linenumber}:%{check}:%{KIND}:%{message}" {} \;
