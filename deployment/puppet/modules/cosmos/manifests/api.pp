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

class cosmos::api inherits cosmos::params {

  include cosmos::setup

  exec { 'install-cosmos-api':
    command   => "yum localinstall -d 0 -e 0 -y ${cosmos_stack_repo_path}/cosmos-api-*.rpm",
    path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
  }

  exec { 'install-cosmos-admin':
    command   => "yum localinstall -d 0 -e 0 -y ${cosmos_stack_repo_path}/cosmos-admin-*.rpm",
    path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
  }

  service { 'cosmos-api':
    ensure     => 'running',
    enable     => true,
    hasstatus  => true,
    hasrestart => true,
  }

  Exec['install-cosmos-api'] -> Service['cosmos-api']

  Class['cosmos::setup']                      ~> Service['cosmos-api']
  File['cosmos-api.conf', 'logback.conf']     ~> Service['cosmos-api']

  anchor { 'cosmos::api::begin': }
    -> Class['cosmos::setup']
    -> anchor { 'cosmos::api::end': }
}
