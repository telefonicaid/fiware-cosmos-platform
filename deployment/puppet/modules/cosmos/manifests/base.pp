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

class cosmos::base inherits cosmos::params {
  include cosmos::cluster_hosts

  yumrepo { 'cosmos-repo' :
    name     => 'cosmos-repo',
    baseurl  => "${cosmos_repo_url}/cosmos",
    gpgcheck => '0',
    enabled  => '1',
  }

  package { 'java-1.7.0-openjdk':
    ensure => installed,
  }

  file { '/etc/puppet':
    ensure => directory,
    owner  => 'root',
    group  => 'root'
  }

  file { '/etc/puppet/puppet.conf':
    source  => 'puppet:///modules/cosmos/puppet.conf',
    owner   => 'root',
    group   => 'root',
  }
  File['/etc/puppet'] -> File['/etc/puppet/puppet.conf']

  anchor { 'cosmos::base::begin': }
    -> Class['cosmos::cluster_hosts']
    -> anchor { 'cosmos::base::end': }
}
