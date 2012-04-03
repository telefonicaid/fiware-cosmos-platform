#!/bin/bash
# TODO: include in the buildout stuff
thrift -r --gen py:new_style ../../../../../cluster/server/src/main/thrift/cluster.thrift
