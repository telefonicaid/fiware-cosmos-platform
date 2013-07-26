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

class ambari::config {
  file_line { 'remove original jdk bin ':
    ensure => 'absent',
    line   => 'jdk.url=http://public-repo-1.hortonworks.com/ARTIFACTS/jdk-6u31-linux-x64.bin',
    path   => '/etc/ambari-server/conf/ambari.properties',
  }

  file_line { 'add jdk bin from CI':
    ensure => 'present',
    line   => "jdk.url=${ambari::params::jdk_url}",
    path   => '/etc/ambari-server/conf/ambari.properties',
  }

  exec { 'ambari-server-setup':
    command   => 'ambari-server setup --silent',
    path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
    logoutput => true,
    timeout   => 600,
    # ensure tables exist
    unless    => 'sudo -u postgres psql -l | grep ambari | wc -l | grep 2'
  }

  File_line['remove original jdk bin ']
    -> File_line['add jdk bin from CI']
    -> Exec['ambari-server-setup']
}
