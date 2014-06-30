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
