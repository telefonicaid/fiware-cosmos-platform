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

cat >test.txt <<EOB
To Infinity and beyond!
EOB

hdfs dfs -copyFromLocal test.txt hdfs:///user/${USER}/
hdfs dfs -get hdfs:///user/${USER}/test.txt test_from_hdfs.txt
diff test.txt test_from_hdfs.txt --brief
