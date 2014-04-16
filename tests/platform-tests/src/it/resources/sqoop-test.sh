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

HOST=`hostname`
DB_URI=jdbc:mysql://${HOSTNAME}/testdb

# Create a fresh, schema, user, tables and 3 sample records
cat >schema.sql <<EOB
create schema if not exists testdb;
use testdb;
drop user 'user'@'%';
create user 'user'@'%';
grant all privileges on testdb.* to 'user'@'%';
flush privileges;
drop table if exists points;
drop table if exists pointscopy;
create table points (x int, y int);
create table pointscopy (x int, y int);
insert into points (x, y) values (10, 10), (20, 30), (-3, 15);
EOB

sudo yum -y install mysql mysql-server
sudo service mysqld start

mysql -u root < schema.sql

hdfs dfs -test -e points && hdfs dfs -rm -r points
sqoop import --connect ${DB_URI} --table points --username user --direct \
	--split-by x
sqoop export --connect ${DB_URI} --table pointscopy --export-dir points \
	--username user

# We should have the same 3 records in both tables
# Note that grep has exit code 1 when no line matches
echo "select count(*) from points p1, pointscopy p2 where p1.x = p2.x and p1.y = p2.y;" | mysql -u root testdb | grep 3
