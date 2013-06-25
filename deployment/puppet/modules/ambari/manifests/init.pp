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
    refreshonly => true,
    timeout     => 600,
  }                # Relation between this and service start

  service { 'ambari-server':
    ensure    => "running",
    hasstatus => true,
    require  => Package['ambari-server'],
  }

  Package['ambari-server'] ~> Exec['ambari-server-setup'] ~> Service['ambari-server']
  Yumrepo['ambari-1.x'] -> Package['ambari-server'] -> Service['ambari-server']
}
