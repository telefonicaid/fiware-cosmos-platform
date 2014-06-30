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

: ${WORKSPACE:?'Need to set $WORKSPACE variable to the Jenkins job workspace dir'}
set -e

if [ $# -ne 1 ]; then
    echo "Please indicate the scripts folder e.g. pre-build"
    exit 1
fi

if [ ! -d $1 ]; then
  echo "Directory '$1' does not exist"
  exit 1
fi

echo "==== Executing scripts in '$1' directory ===="
for file in $1/*
do
    echo "* Executing $file ..."
    $file
done
echo '==== Finished processing ===='
