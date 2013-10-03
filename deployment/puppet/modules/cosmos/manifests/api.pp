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

  package { 'cosmos':
    ensure => latest,
  }

  file { $cosmos::params::cosmos_cli_repo_path:
    ensure => 'directory'
  }

  file { "${cosmos::params::cosmos_cli_repo_path}/eggs":
    ensure => 'directory',
    owner  => 'root',
    group  => 'root',
    mode   => '0755'
  }

  service { 'cosmos-api':
    ensure     => 'running',
    enable     => true,
    hasstatus  => true,
    hasrestart => true,
  }

  YumRepo['cosmos'] -> Package['cosmos'] -> Service['cosmos-api']
  Class['cosmos::setup']                      ~> Service['cosmos-api']
  File['cosmos-api.conf', 'logback.conf']     ~> Service['cosmos-api']

  anchor { 'cosmos::api::begin': }
    -> Class['cosmos::setup']
    -> anchor { 'cosmos::api::end': }
}
