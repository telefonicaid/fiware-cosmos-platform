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
