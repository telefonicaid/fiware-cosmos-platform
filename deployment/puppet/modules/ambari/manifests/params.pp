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

class ambari::params (
  $ambari_repo_url      = 'http://cosmos10/develenv/repos/rpms/cosmos-deps/ambari/',#'http://192.168.63.11:8081/',
  $agent_package_ver	  = 'installed',
  $server_package_ver   = 'installed',
) {

}
