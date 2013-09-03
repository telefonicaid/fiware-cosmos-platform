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

if [ $# -ne 1 ]; then
    echo "Please indicate the scripts folder e.g. pre-build"
    exit 1
fi

if [ ! -d $1 ]; then
  echo "Directory '$1' does not exist"
  exit 1
fi

echo "==== Executing scripts in '$1' directory ===="
for file in $1/*
do
    echo "* Executing $file ..."
    ./$file
done
echo '==== Finished processing ===='
