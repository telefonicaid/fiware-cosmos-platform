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

class ambari::server::config {
  file_line { 'remove original jdk bin':
    ensure => 'absent',
    line   => 'jdk.url=http://public-repo-1.hortonworks.com/ARTIFACTS/jdk-6u31-linux-x64.bin',
    path   => '/etc/ambari-server/conf/ambari.properties',
  }

  file_line { 'add jdk bin from CI':
    ensure => 'present',
    line   => "jdk.url=${ambari::params::jdk_url}",
    path   => '/etc/ambari-server/conf/ambari.properties',
  }

  $tables_exist = '/bin/bash -c "[[ `sudo -u postgres psql -l | grep ambari | wc -l` -ge 2 ]]"'

  exec { 'ambari-server-stop':
    command   => 'ambari-server stop',
    path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
    logoutput => true,
    timeout   => 600,
    onlyif    => $tables_exist
  }

  exec { 'ambari-server-setup':
    command   => 'ambari-server setup --silent',
    path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
    logoutput => true,
    timeout   => 600,
    unless    => $tables_exist
  }

  exec { 'ambari-server-upgrade':
    command   => 'ambari-server upgrade --silent',
    path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
    logoutput => true,
    timeout   => 600,
    onlyif    => $tables_exist
  }

  augeas { "ambari-config-repoinfo":
    lens    => "Xml.lns",
    incl    => "/var/lib/ambari-server/resources/stacks/HDP/2.0.6_Cosmos/repos/repoinfo.xml",
    changes => [
    "set reposinfo/os/repo/baseurl/#text ${ambari::params::repo_rpm_url}"
    ],
  }

  File_line['remove original jdk bin']
    -> File_line['add jdk bin from CI']
    -> Augeas['ambari-config-repoinfo']
    -> Exec['ambari-server-stop']
    -> Exec['ambari-server-upgrade']
    -> Exec['ambari-server-setup']
}
