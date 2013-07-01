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

class ambari {
  include ambari::ambari_repos

  package { 'ambari-server':
    ensure  => installed,
  }

  exec { 'ambari-server-setup':
    command     => 'ambari-server setup --silent',
    path        => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
    logoutput   => true,
    timeout     => 600,
    unless      => 'sudo -u postgres psql -l | grep ambari | wc -l | grep 2' #ensure tables exist
  }

  service { 'postgresql':
    ensure => 'running',
    hasstatus => true,
  }

  service { 'ambari-server':
    ensure    => 'running',
    hasstatus => true,
  }

#  Package['ambari-server'] ~> Exec['ambari-server-setup'] ~> Service['ambari-server']
  Yumrepo['ambari'] -> Package['ambari-server'] -> Exec['ambari-server-setup'] -> Service['postgresql'] -> Service['ambari-server']
}
