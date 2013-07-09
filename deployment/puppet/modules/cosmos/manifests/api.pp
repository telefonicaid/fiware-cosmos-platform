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

  mysql::db { "${cosmos_db_name}":
    user     => "${cosmos_db_user}",
    password => "${cosmos_db_pass}",
    host     => '%',
    grant    => ['all']
  }

  database_user { "${cosmos_db_user}@localhost":
    password_hash => mysql_password("${cosmos_db_pass}")
  }

  database_grant { "${cosmos_db_user}@localhost/${cosmos_db_name}":
    privileges => ['all']
  }

  package { 'cosmos':
    ensure => latest,
  }

  file { "${$cosmos_cli_repo_path}/eggs":
    ensure => 'directory',
    owner  => 'root',
    group  => 'root',
  }

  wget::fetch { 'download cosmos-cli':
    source      => "${cosmos_egg_repo}/${cosmos_cli_egg}",
    destination => "${$cosmos_cli_repo_path}/eggs/${cosmos_cli_egg}",
  }

  service { 'cosmos-api':
    ensure     => 'running',
    enable     => true,
    hasstatus  => true,
    hasrestart => true,
  }

  YumRepo['cosmos-repo'] -> Package['cosmos'] -> Service['cosmos-api']
  Class['mysql::server']                      -> Service['cosmos-api']
  Exec['cosmos-setup']                        ~> Service['cosmos-api']
  File['cosmos-api.conf', 'logback.conf']     ~> Service['cosmos-api']

  File[$cosmos_cli_repo_path]
    -> File["${$cosmos_cli_repo_path}/eggs"]
    -> Wget::Fetch['download cosmos-cli']

  anchor { 'cosmos::api::begin': }
    -> Class['cosmos::setup']
    -> anchor { 'cosmos::api::end': }
}
