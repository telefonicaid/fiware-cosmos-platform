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

cat >job.properties <<EOB
oozie.services.ext=org.apache.oozie.service.HadoopAccessorService
nameNode=hdfs://`hostname`:8020
jobTracker=`hostname`:8050
queueName=default
oozie.wf.application.path=/user/${USER}/teragen.xml
EOB

cat >teragen.xml <<EOB
<workflow-app name="teragen" xmlns="uri:oozie:workflow:0.1">
  <start to="a1"/>
  <action name="a1">
    <map-reduce>
      <job-tracker>\${jobTracker}</job-tracker>
      <name-node>\${nameNode}</name-node>
      <configuration>
        <property>
          <name>terasort.num-rows</name><value>10</value>
        </property>
        <property>
          <name>mapred.output.dir</name>
          <value>\${nameNode}/user/${USER}/dummy</value>
        </property>
        <property>
          <name>mapred.reduce.tasks</name>
          <value>0</value>
        </property>
        <property>
          <name>mapreduce.job.inputformat.class</name>
          <value>org.apache.hadoop.examples.terasort.TeraGen\$RangeInputFormat</value>
        </property>
        <property>
          <name>mapred.mapper.new-api</name>
          <value>true</value>
        </property>
      </configuration>
    </map-reduce>
    <ok to="finish" />
    <error to="fail" />
  </action>
  <kill name="fail">
    <message>FAILED!</message>
  </kill>
  <end name="finish" />
</workflow-app>
EOB

hdfs dfs -test -f teragen.xml && hdfs dfs -rm teragen.xml
hdfs dfs -test -d lib && hdfs dfs -rm -r lib
hdfs dfs -test -d dummy && hdfs dfs -rm -r dummy

hdfs dfs -put teragen.xml hdfs:///user/${USER}/
hdfs dfs -mkdir lib
hdfs dfs -put /usr/lib/hadoop-mapreduce/hadoop-mapreduce-examples*.jar lib/

OOZIE_OPTS="-oozie http://localhost:11000/oozie"
OOZIE_OUT=`oozie job $OOZIE_OPTS -config job.properties -auth SIMPLE -run`
if [ $? != "0" ]; then
  echo "Oozie command failed: $OOZIE_OUT"
  exit $?
fi
JOB_ID=`echo $OOZIE_OUT | awk '{ print $2 }'`

echo "JOB is $JOB_ID"
while true; do
  JOB_STATUS=`oozie job $OOZIE_OPTS -info $JOB_ID | grep ^Status | cut -d: -f2 | tr -d ' '`
  case $JOB_STATUS in
    RUNNING )
      echo "Job is still running, waiting for completion..."
      sleep 5
      ;;
    SUCCEEDED )
      echo "Job succeeded"
      hdfs dfs -test -f dummy/_SUCCESS
      exit
      ;;
    *)
      echo "The job reached a unexpected status $JOB_STATUS"
      exit 1
  esac
done
