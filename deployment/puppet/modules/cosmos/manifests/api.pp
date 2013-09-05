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

  mysql::db { $cosmos::params::cosmos_db_name:
    user     => $cosmos::params::cosmos_db_user,
    password => $cosmos::params::cosmos_db_pass,
    host     => '%',
    grant    => ['all']
  }

  database_user { "${cosmos::params::cosmos_db_user}@${cosmos::params::cosmos_db_host}":
    password_hash => mysql_password($cosmos::params::cosmos_db_pass)
  }

  database_grant { "${cosmos::params::cosmos_db_user}@${cosmos::params::cosmos_db_host}/${cosmos::params::cosmos_db_name}":
    privileges => ['all']
  }

  package { 'cosmos':
    ensure => latest,
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
  Class['mysql::server']                      -> Service['cosmos-api']
  Exec['cosmos-setup']                        ~> Service['cosmos-api']
  File['cosmos-api.conf', 'logback.conf']     ~> Service['cosmos-api']

  anchor { 'cosmos::api::begin': }
    -> Class['cosmos::setup']
    -> anchor { 'cosmos::api::end': }
}
