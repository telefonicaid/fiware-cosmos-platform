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

  $ial_schema = '/tmp/ial_schema.sql'
  $ial_machines = '/tmp/ial_machines.sql'

  file { "${ial_schema}":
    ensure => present,
    source => 'puppet:///modules/cosmos/ial_schema.sql'
  }

  file { "${ial_machines}":
    ensure => present,
    source => "puppet:///modules/cosmos/environments/${environment}/ial_machines.sql"
  }

  exec { 'ial_db':
    command => "cat ${ial_schema} ${ial_machines} | mysql -ucosmos -p${cosmos_db_pass} cosmos -B",
    path => $path,
    require => [
      File["${ial_schema}"],
      File["${ial_machines}"],
      Database["${cosmos_db_name}"]
    ]
  }

  exec { 'cosmos-setup':
    command => '/opt/pdi-cosmos/cosmos-admin/cosmos-admin setup',
    require => [
      Package['cosmos'],
      Exec['ial_db']
    ],
  }
}