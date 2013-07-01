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

class cosmos::base {
  include pdi_base
  include cosmos::cluster_hosts
  include cosmos::params

  service {'iptables':
    ensure	=> stopped
  }

  package {'java-1.7.0-openjdk':
    ensure => installed
  }

  file { '/etc/puppet':
    ensure => directory,
    owner  => 'root',
    group  => 'root'
  }

  file { '/etc/puppet/puppet.conf':
    source  => 'puppet:///modules/cosmos/puppet.conf',
    owner  => 'root',
    group  => 'root',
    require => File['/etc/puppet']
  }
}
