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

  include cosmos::master_db

  package { ['libvirt-client', 'libvirt-java'] :
    ensure => 'present'
  }

  file { 'cosmos-api.conf':
    ensure  => 'present',
    path    => "${cosmos::params::cosmos_confdir}/cosmos-api.conf",
    mode    => '0644',
    content => template('cosmos/cosmos-api.conf.erb'),
  }

  file { 'ial.conf':
    ensure  => 'present',
    path    => "${cosmos::params::cosmos_confdir}/ial.conf",
    mode    => '0644',
    content => template('cosmos/ial.conf.erb'),
  }

  file { 'service configurations':
    ensure  => 'directory',
    source  => "puppet:///modules/${module_name}/services",
    path    => "${cosmos::params::cosmos_confdir}/services",
    owner   => 'root',
    group   => 'root',
    mode    => '0640',
    recurse => true
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

  File[$cosmos::params::cosmos_confdir] -> Class['cosmos::master_db']
  File[$cosmos::params::cosmos_confdir]
    -> File[
        'cosmos-api.conf',
        'ial.conf',
        'service configurations',
        'logback.conf'
      ]

  Package['cosmos-api'] -> File['cosmos-api.conf']

  Package['cosmos-admin'] -> Exec['cosmos-setup']

  Apache::Vhost['platform.repo'] -> Exec['cosmos-setup']

  Class['ssh_keys', 'cosmos::master_db'] -> Exec['cosmos-setup']

  File[
    'cosmos-api.conf',
    $cosmos::params::ssl_cert_file,
    $cosmos::params::ssl_key_file,
    $cosmos::params::ssl_ca_file
  ] ~> Exec['cosmos-setup']

  anchor{'cosmos::setup::begin': }
    -> Class['cosmos::master_db']
    -> anchor{'cosmos::setup::end': }
}
