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

class ambari_repos::params {

  $ambari_repo_base = 'http://cosmos10/develenv/repos/rpms/cosmos-deps/ambari'
  
  case $environment {
    'vagrant', 'andromeda' : { $ambari_repo_url = "${ambari_repo_base}/develop" }
    'orion' : { $ambari_repo_url = "${ambari_repo_base}/master" }
    default : { $ambari_repo_url = "${ambari_repo_base}/master" }
  }
}
