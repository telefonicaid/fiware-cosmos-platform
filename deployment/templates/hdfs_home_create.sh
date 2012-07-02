#!/bin/bash
su hdfs -c "hadoop dfs -mkdir /user/$1/{datasets,jars,tmp}"
su hdfs -c "hadoop dfs -chown -R $1:hue /user/$1"
