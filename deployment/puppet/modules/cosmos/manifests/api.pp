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
  include mysql

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
    ensure  => latest,
    require => YumRepo['cosmos-repo']
  }

  file { "${cosmos_confdir}":
    ensure => 'directory',
    mode   => '0440',
  }

  file { 'cosmos-api.conf':
    path    => "${cosmos_confdir}/cosmos-api.conf",
    ensure  => 'present',
    mode    => '0644',
    content => template("cosmos/cosmos-api.conf.erb"),
    require => [Package['cosmos'], File[$cosmos_confdir]]
  }

  file { 'logback.conf' :
    path    => "${cosmos_confdir}/logback.conf",
    ensure  => 'present',
    mode    => '0644',
    content => template('cosmos/logback.conf.erb'),
    require => File[$cosmos_confdir],
  }

  service { 'cosmos-api':
    ensure => 'running',
    enable => true,
    require => [
      Package['cosmos'],
      File['cosmos-api.conf'],
      File['logback.conf'],
      Class['mysql::server']
    ]
  }
}
