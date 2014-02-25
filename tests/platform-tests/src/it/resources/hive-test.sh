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

cat >points.csv <<EOB
10,10
5,15
15,24
-3,2
EOB
hdfs dfs -test -d csv || hdfs dfs -mkdir csv
hdfs dfs -test -e csv/points.csv || hdfs dfs -put points.csv csv/points.csv

cat >hive.sql <<EOB
create database if not exists testdb location 'hdfs:///user/${USER}/testdb';
drop table if exists points;
create external table points (x int, y int) row format delimited fields terminated by ',' lines terminated by '\n' stored as textfile location 'hdfs:///user/${USER}/csv';
insert overwrite directory 'hdfs:///user/${USER}/result' select sum(x) + sum(y) from points;
EOB
cat hive.sql | hive

OUT=`hdfs dfs -cat hdfs:///user/${USER}/result/*`
[ $OUT = "78" ]
