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
hdfs dfs -put test.txt infinity:///${USER}/test.txt
hdfs dfs -chmod 700 infinity:///${USER}/test.txt
hdfs dfs -get infinity:///${USER}/test.txt test2.txt
diff test.txt test2.txt
hdfs dfs -ls infinity:///${USER}/
hdfs dfs -ls infinity:///${USER}/test.txt
hdfs dfs -mkdir infinity:///${USER}/dummy_dir
hdfs dfs -mv infinity:///${USER}/test.txt infinity:///${USER}/test2.txt
hdfs dfs -cp infinity:///${USER}/test2.txt infinity:///${USER}/dummy_dir/test3.txt
hdfs dfs -rm -r infinity:///${USER}/dummy_dir
hdfs dfs -rm infinity:///${USER}/test*

hdfs dfs -get infinity:///${USER}/onlyUser.txt .
hdfs dfs -get infinity:///${USER}/onlyGroup.txt .
hdfs dfs -get infinity:///${USER}/onlyEveryone.txt .
