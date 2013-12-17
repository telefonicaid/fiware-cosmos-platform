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
  $repo_url,
  $hdp_utils_repo_url = 'http://servilinux.hi.inet/ambari/HDP-UTILS-1.1.0.15/',
  $hdp_utils_gpg_url = 'http://servilinux.hi.inet/ambari/RPM-GPG-KEY-Jenkins',
  $install_ensure  = 'latest',
  $jdk_url         = 'http://cosmos10/develenv/repos/bins/jdk-6u31-linux-x64.bin'
) {
}
