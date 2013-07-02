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

class cosmos::master {
  include pdi_base, ssh_keys, cosmos::cluster_hosts, ambari, mysql

  service {'iptables':
    ensure	=> stopped,
  }

  # Java JDK
  package {'jdk':
    name   => 'java-1.7.0-openjdk',
    ensure => installed,
  }

  # class { 'mysql': }

  class { 'mysql::server':
    config_hash => { 'root_password' => 'cosmos' }
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

  mysql::db { 'cosmos':
    user     => 'cosmos',
    password => 'cosmos',
    host     => '%',
    grant    => ['all'],
  }

  # database_user{ 'cosmos@%':
  #   ensure        => present,
  #   password_hash => mysql_password('cosmos'),
  #   require       => Class['mysql::server'],
  # }

  # database_grant { 'cosmos@%/cosmosdb':
  #   privileges => ['all'] ,
  # }

  #host { 'slave1':
  #  ip => '192.168.10.21',
  #  host_aliases => 'cosmos.slave1',
  #}
  #
  #host { 'slave2':
  #  ip => '192.168.10.22',
  #  host_aliases => 'cosmos.slave2',
  #}

}
