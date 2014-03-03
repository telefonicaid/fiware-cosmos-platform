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

class ambari::server::service {
  service { 'ambari-server':
    ensure     => 'running',
    enable     => true,
    hasstatus  => true,
    hasrestart => true,
  }

  exec { 'Wait for server' :
  	command => '/bin/bash -c "while ! nc -vz localhost 8080; do sleep 1; done"',
  	path    => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
  }

  package { 'nc':
    ensure  => present,
  }

  Service['ambari-server'] -> Exec['Wait for server']
  Package['nc'] -> Exec['Wait for server']
}
