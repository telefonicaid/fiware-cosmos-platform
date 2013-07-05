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

# FIXME rename
class cosmos::master_setup inherits cosmos::params {

  anchor {'cosmos::master_setup::begin': }
  anchor {'cosmos::master_setup::end': }

  file { $cosmos_cli_repo_path:
    ensure => 'directory'
  }

  # Apache HTTPD on port 8000 for auxiliary uses

  include apache

  apache::vhost { 'localhost':
    priority => '20',
    port     => '8000',
    docroot  => $cosmos_cli_repo_path,
    require  => File[$cosmos_cli_repo_path]
  }

  file_line { "don't listen on 80 port":
    ensure => 'absent',
    line => 'Listen 80',
    path => '/etc/httpd/conf/httpd.conf',
  } -> file_line { 'listen on 8000 port':
    ensure => 'present',
    line => 'Listen 8000',
    path => '/etc/httpd/conf/httpd.conf',
    notify => Service['httpd']
  }

  # class { 'mysql': }

  class { 'mysql::server':
    config_hash => { 'root_password' => 'cosmos' },
  }

  mysql::server::config { 'basic_config':
    settings => {
      'mysqld' => {
        'bind-address' => '0.0.0.0',
        #'read-only'    => true,
      }#,
      #'client' => {
      #  'port' => '3306'
      #}
    },
  }

  $libvirt_packages = ['libvirt-client', 'libvirt-java']
  package { $libvirt_packages :
    ensure => 'present'
  }

  Anchor['cosmos::master_setup::begin']
  ->
  Class['apache', 'mysql::server']
  ->
  Anchor['cosmos::master_setup::end']
}
