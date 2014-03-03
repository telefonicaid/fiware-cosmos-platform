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
HADOOP_EXAMPLES_JAR=/usr/lib/hadoop-mapreduce/hadoop-mapreduce-examples*.jar
TERAGEN_SIZE=10000
TERASORT_INPUT=hdfs:///user/${USER}/terasort-input
TERASORT_OUTPUT=hdfs:///user/${USER}/terasort-output
TERASORT_VALIDATE=hdfs:///user/${USER}/terasort-validate

hadoop jar $HADOOP_EXAMPLES_JAR teragen $TERAGEN_SIZE $TERASORT_INPUT && \
hadoop jar $HADOOP_EXAMPLES_JAR terasort $TERASORT_INPUT $TERASORT_OUTPUT && \
hadoop jar $HADOOP_EXAMPLES_JAR teravalidate $TERASORT_OUTPUT $TERASORT_VALIDATE

OUT=`hdfs dfs -cat ${TERASORT_VALIDATE}/part-r-00000`
echo "Output file content is $OUT"
[[ $OUT == "checksum	139abefd74b2" ]]
