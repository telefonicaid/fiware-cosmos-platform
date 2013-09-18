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

class cosmos::master_setup inherits cosmos::params {

  file { $cosmos::params::cosmos_cli_repo_path:
    ensure => 'directory'
  }

  # Apache HTTPD on port 8000 for auxiliary uses

  include apache

  apache::vhost { 'localhost':
    priority => '20',
    port     => '8000',
    docroot  => $cosmos::params::cosmos_cli_repo_path,
  }

  file_line { 'do not listen on 80 port':
    ensure => 'absent',
    line   => 'Listen 80',
    path   => '/etc/httpd/conf/httpd.conf',
  }

  file_line { 'listen on 8000 port':
    ensure => 'present',
    line   => 'Listen 8000',
    path   => '/etc/httpd/conf/httpd.conf',
  }

  class { 'mysql::server':
    config_hash => { 'root_password' => 'cosmos' },
  }

  mysql::server::config { 'basic_config':
    settings => {
      'mysqld' => {
        'bind-address' => '0.0.0.0',
      }
    },
  }

  package { ['libvirt-client', 'libvirt-java'] :
    ensure => 'present'
  }

  File[$cosmos::params::cosmos_cli_repo_path] -> Apache::Vhost['localhost']

  File_line['do not listen on 80 port']
    -> File_line['listen on 8000 port']
    ~> Service['httpd']

  anchor{'cosmos::master_setup::begin': }
    -> Class['apache', 'mysql::server']
    -> anchor{'cosmos::master_setup::end': }
}
