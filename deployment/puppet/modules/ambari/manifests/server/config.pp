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

  file_line { 'remove original jce_policy file':
    ensure => 'absent',
    line   => 'jce_policy.url=http://public-repo-1.hortonworks.com/ARTIFACTS/jce_policy-6.zip',
    path   => '/etc/ambari-server/conf/ambari.properties',
  }

  file_line { 'add jce_policy from CI':
    ensure => 'present',
    line   => "jce_policy.url=${ambari::params::jce_url}",
    path   => '/etc/ambari-server/conf/ambari.properties',
  }

  exec { 'ambari-server-stop':
    command   => 'ambari-server stop',
    path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
    logoutput => true,
    timeout   => 600
  }

  exec { 'ambari-server-setup':
    command   => 'ambari-server setup --silent',
    path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
    logoutput => true,
    timeout   => 600
  }

  exec { 'ambari-server-upgrade':
    command   => 'ambari-server upgrade --silent',
    path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
    logoutput => true,
    timeout   => 600
  }

  if $ambari::params::reposync {
    augeas { 'ambari-config-repoinfo':
      lens    => 'Xml.lns',
      incl    => '/var/lib/ambari-server/resources/stacks/HDP/2.0.6_Cosmos/repos/repoinfo.xml',
      changes => [
      "rm reposinfo/os[#attribute/type='centos6']/repo[repoid/#text='HDP-2.0.6']",
      "rm reposinfo/os[#attribute/type='centos6']/repo[repoid/#text='cosmos-platform']"
      ],
    }
  } else {
    augeas { "ambari-config-repoinfo":
      lens    => "Xml.lns",
      incl    => "/var/lib/ambari-server/resources/stacks/HDP/2.0.6_Cosmos/repos/repoinfo.xml",
      changes => [
      "set reposinfo/os[#attribute/type='centos6']/repo[repoid/#text='HDP-2.0.6']/repoid/#text HDP-2.0.6",
      "set reposinfo/os[#attribute/type='centos6']/repo[repoid/#text='HDP-2.0.6']/baseurl/#text ${ambari::params::hdp_stack_repo_url}",
      "set reposinfo/os[#attribute/type='centos6']/repo[repoid/#text='HDP-2.0.6']/reponame/#text HDP-2.0.6",
      "set reposinfo/os[#attribute/type='centos6']/repo[repoid/#text='cosmos-platform']/repoid/#text cosmos-platform",
      "set reposinfo/os[#attribute/type='centos6']/repo[repoid/#text='cosmos-platform']/baseurl/#text ${ambari::params::cosmos_stack_repo_url}",
      "set reposinfo/os[#attribute/type='centos6']/repo[repoid/#text='cosmos-platform']/reponame/#text cosmos-platform"
      ],
    }
  }

  File_line['remove original jdk bin']
    -> File_line['add jdk bin from CI']
    -> File_line['remove original jce_policy file']
    -> File_line['add jce_policy from CI']
    -> Augeas['ambari-config-repoinfo']
    -> Exec['ambari-server-stop']
    -> Exec['ambari-server-setup']
    -> Exec['ambari-server-upgrade']
}
