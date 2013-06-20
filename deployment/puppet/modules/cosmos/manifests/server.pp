# Server Class for Cosmos
#
#
# This module will install ambari-server from the Cosmos platform

class cosmos::server (

  $svc_enable     = 'true',
  $svc_ensure     = 'running',

  ) inherits cosmos {

  if ( $cosmos_repo  == yes) {
    include cosmos::common::repos
  }

  include cosmos::config::api

  $pkg_server     = 'ambari-server'
  $pkg_web        = 'ambari-web'

  package { "${pkg_server}" :
    ensure  => 'installed',
  }
  
  package { "${pkg_ambari}" :
    ensure  => 'installed',
  }
  
  package { "${pkg_web}" :
    ensure  => 'installed',
  }

  file { 'cosmos-api.conf' :
    ensure  => 'present',
    path    => "${cosmos_conf_dir}/${service_name}.conf",
    mode    => '0755',
    owner   => "${name}",
    group   => "${name}",
    content => template("cosmos/${service_name}.conf.erb"),
  }

  file { 'logback.conf' :
    ensure  => 'present',
    path    => "${cosmos_conf_dir}/logback.conf",
    mode    => '0755',
    owner   => 'cosmos',
    group   => 'cosmos',
    content => template('cosmos/logback.conf.erb'),
  }

  Package["${pkg_ambari}"] -> Package["${pkg_server}"] -> Package["${pkg_web}"] -> File["${service_name}"] -> Service["${service_name}"] -> File['cosmos-api.conf'] -> File['logback.conf']

}
