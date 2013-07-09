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

  $ial_schema   = "${cosmos_basedir}/ial/ial_schema.sql"
  $ial_machines = "${cosmos_basedir}/ial/ial_machines.sql"

  file { 'ial':
    path   => "${cosmos_basedir}/ial",
    ensure => 'directory',
  }

  file { "${ial_schema}":
    ensure => present,
    source => 'puppet:///modules/cosmos/ial_schema.sql',
  }

  file { "${ial_machines}":
    ensure => present,
    source => "puppet:///modules/cosmos/environments/${environment}/ial_machines.sql",
  }

  exec { 'ial_db':
    command     => "cat ${ial_schema} ${ial_machines} | mysql -ucosmos -p${cosmos_db_pass} cosmos -B",
    path        => $path,
    refreshonly => true,
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
  }

  file { 'logback.conf' :
    path    => "${cosmos_confdir}/logback.conf",
    ensure  => 'present',
    mode    => '0644',
    content => template('cosmos/logback.conf.erb'),
  }

  exec { 'cosmos-setup':
    command     => "${cosmos_basedir}/cosmos-admin/cosmos-admin setup",
    refreshonly => true,
    user        => root,
    timeout     => 900,
  }

  File['ial'] -> File["${ial_schema}", "${ial_machines}"] ~> Exec['ial_db']
  Database["${cosmos_db_name}"]                           ~> Exec['ial_db']

  File[$cosmos_confdir] -> File['cosmos-api.conf', 'logback.conf']
  Package['cosmos']     -> File['cosmos-api.conf']

  Class['ssh_keys']       -> Exec['cosmos-setup']
  Package['cosmos']       ~> Exec['cosmos-setup']
  Exec['ial_db']          ~> Exec['cosmos-setup']
  File['cosmos-api.conf'] ~> Exec['cosmos-setup']
}
