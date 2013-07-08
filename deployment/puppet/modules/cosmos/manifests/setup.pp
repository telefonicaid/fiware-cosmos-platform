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
    path => "${cosmos_basedir}/ial",
    ensure => 'directory',
  }

  file { "${ial_schema}":
    ensure  => present,
    source  => 'puppet:///modules/cosmos/ial_schema.sql',
    require => File['ial'],
  }

  file { "${ial_machines}":
    ensure  => present,
    source  => "puppet:///modules/cosmos/environments/${environment}/ial_machines.sql",
    require => File['ial'],
  }

  exec { 'ial_db':
    command     => "cat ${ial_schema} ${ial_machines} | mysql -ucosmos -p${cosmos_db_pass} cosmos -B",
    path        => $path,
    refreshonly => true,
    subscribe   => [
      File["${ial_schema}"],
      File["${ial_machines}"],
      Database["${cosmos_db_name}"]
    ]
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

  exec { 'cosmos-setup':
    command => '/opt/pdi-cosmos/cosmos-admin/cosmos-admin setup',
    refreshonly => true,
    require => Class['ssh_keys'],
    user => root,
    subscribe => [
      Package['cosmos'],
      Exec['ial_db'],
      File['cosmos-api.conf'],
    ],
  }
}