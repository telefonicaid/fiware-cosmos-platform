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

HADOOP_EXAMPLES_JAR=/usr/lib/hadoop-mapreduce/hadoop-mapreduce-examples*.jar
TERAGEN_SIZE=10000
TERASORT_INPUT=infinity:///${USER}/terasort-input
TERASORT_OUTPUT=infinity:///${USER}/terasort-output
TERASORT_VALIDATE=infinity:///${USER}/terasort-validate

hadoop jar $HADOOP_EXAMPLES_JAR teragen $TERAGEN_SIZE $TERASORT_INPUT && \
hadoop jar $HADOOP_EXAMPLES_JAR terasort $TERASORT_INPUT $TERASORT_OUTPUT && \
hadoop jar $HADOOP_EXAMPLES_JAR teravalidate $TERASORT_OUTPUT $TERASORT_VALIDATE

OUT=`hdfs dfs -cat ${TERASORT_VALIDATE}/part-r-00000`
echo "Output file content is $OUT"
[[ $OUT == "checksum	139abefd74b2" ]]
