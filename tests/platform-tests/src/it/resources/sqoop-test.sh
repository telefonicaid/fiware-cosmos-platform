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
