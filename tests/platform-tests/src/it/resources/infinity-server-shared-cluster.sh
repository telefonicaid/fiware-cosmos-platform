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

echo "Test file" > test.txt
hdfs dfs -put test.txt infinity:///${TARGET_USER}/test.txt
hdfs dfs -chmod 700 infinity:///${TARGET_USER}/test.txt
hdfs dfs -get infinity:///${TARGET_USER}/test.txt test2.txt
diff test.txt test2.txt
hdfs dfs -ls infinity:///${TARGET_USER}/
hdfs dfs -ls infinity:///${TARGET_USER}/test.txt
hdfs dfs -mkdir infinity:///${TARGET_USER}/dummy_dir
hdfs dfs -mv infinity:///${TARGET_USER}/test.txt infinity:///${TARGET_USER}/test2.txt
hdfs dfs -cp infinity:///${TARGET_USER}/test2.txt infinity:///${TARGET_USER}/dummy_dir/test3.txt
hdfs dfs -rm -r infinity:///${TARGET_USER}/dummy_dir
hdfs dfs -rm infinity:///${TARGET_USER}/test*

set +e
hdfs dfs -get infinity:///${TARGET_USER}/onlyUser.txt .
if [ $? == "0" ]; then
  echo "Was able to read a user-only file from a shared cluster"
  exit 1
fi
set -e

hdfs dfs -get infinity:///${TARGET_USER}/onlyGroup.txt .
hdfs dfs -get infinity:///${TARGET_USER}/everyone.txt .
