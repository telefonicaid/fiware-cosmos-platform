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

class cosmos::setup inherits cosmos::params {

  $ial_schema   = "${cosmos::params::cosmos_basedir}/ial/ial_schema.sql"
  $ial_machines = "${cosmos::params::cosmos_basedir}/ial/ial_machines.sql"

  file { 'ial':
    ensure => 'directory',
    path   => "${cosmos::params::cosmos_basedir}/ial",
  }

  file { $ial_schema:
    ensure => present,
    source => "puppet:///modules/${module_name}/ial_schema.sql",
  }

  file { $ial_machines:
    ensure  => present,
    content => template('cosmos/ial_machines.sql.erb'),
  }

  exec { 'ial_db':
    command     => "cat ${ial_schema} ${ial_machines} | mysql -ucosmos -p${cosmos::params::cosmos_db_pass} cosmos -B",
    path        => $::path,
    refreshonly => true,
  }

  file { $cosmos::params::cosmos_confdir:
    ensure => 'directory',
    mode   => '0440',
  }

  file { 'cosmos-api.conf':
    ensure  => 'present',
    path    => "${cosmos::params::cosmos_confdir}/cosmos-api.conf",
    mode    => '0644',
    content => template('cosmos/cosmos-api.conf.erb'),
  }

  file { 'logback.conf' :
    ensure  => 'present',
    path    => "${cosmos::params::cosmos_confdir}/logback.conf",
    mode    => '0644',
    content => template('cosmos/logback.conf.erb'),
  }

  exec { 'cosmos-setup':
    command     => "${cosmos::params::cosmos_basedir}/cosmos-admin/cosmos-admin setup",
    refreshonly => true,
    user        => root,
    timeout     => 900,
  }

  File['ial'] -> File[$ial_schema, $ial_machines] ~> Exec['ial_db']
  Database[$cosmos::params::cosmos_db_name]       ~> Exec['ial_db']

  File[$cosmos::params::cosmos_confdir] -> File['cosmos-api.conf', 'logback.conf']
  Package['cosmos']                     -> File['cosmos-api.conf']

  Class['ssh_keys']       -> Exec['cosmos-setup']
  Package['cosmos']       ~> Exec['cosmos-setup']
  Exec['ial_db']          ~> Exec['cosmos-setup']
  File['cosmos-api.conf'] ~> Exec['cosmos-setup']
}
