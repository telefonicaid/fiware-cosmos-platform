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

class cosmos::setup {
  include cosmos::params, cosmos::master_db

  # Files as fake templates to allow to load them in a variable.
  # Puppet does not support loading file contents in variables.
  $ssl_cert_content = template($cosmos::params::infinity_proxy_ssl_cert_source)
  $ssl_key_content  = template($cosmos::params::infinity_proxy_ssl_key_source)

  # Template injection variables
  # Escape new lines so that the can be compatible with JSON syntax.
  # JSON does not allow multiline strings.
  $infinity_proxy_ssl_cert      = regsubst($ssl_cert_content, '\n', '\\n', 'G')
  $infinity_proxy_ssl_key       = regsubst($ssl_key_content, '\n', '\\n', 'G')
  $infinity_proxy_secure_phrase = $cosmos::params::infinity_proxy_secure_phrase

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

  $services_confdir = "${cosmos::params::cosmos_confdir}/services"

  file { 'service configurations directory':
    ensure  => 'directory',
    path    => $services_confdir,
    owner   => 'root',
    group   => 'root',
    mode    => '0640',
  }

  recursive_directory { 'service configurations':
    source_dir => "${module_name}/services",
    dest_dir   => $services_confdir,
    owner      => 'root',
    group      => 'root',
    file_mode  => '0640',
    dir_mode   => '0640',
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
        'service configurations directory',
        'logback.conf'
      ]
  File['service configurations directory'] -> Recursive_directory['service configurations']

  Exec['install-cosmos-api'] -> File['cosmos-api.conf']

  Exec['install-cosmos-admin'] -> Exec['cosmos-setup']

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
