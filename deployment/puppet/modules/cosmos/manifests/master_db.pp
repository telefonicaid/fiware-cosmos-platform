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

class cosmos::master_db inherits cosmos::params {
  class { 'mysql::server':
    config_hash => { 'root_password' => 'cosmos' },
  }

  mysql::server::config { 'basic_config':
    settings => {
      'mysqld' => {
        'bind-address' => '127.0.0.1',
        'default-storage-engine' => 'INNODB'
      }
    },
  }

  mysql::db { $cosmos::params::cosmos_db_name:
    user     => $cosmos::params::cosmos_db_user,
    password => $cosmos::params::cosmos_db_pass,
    host     => '%',
    grant    => ['all']
  }

  $db_user        = "${cosmos::params::cosmos_db_user}@${cosmos::params::cosmos_db_host}"
  $db_user_and_db = "${cosmos::params::cosmos_db_user}@${cosmos::params::cosmos_db_host}/${cosmos::params::cosmos_db_name}"

  database_user { $db_user:
    password_hash => mysql_password($cosmos::params::cosmos_db_pass)
  }

  database_grant { $db_user_and_db :
    privileges => ['all']
  }

  anchor{'cosmos::master_db::begin': }
    -> Class['mysql::server']
    -> Database[$cosmos::params::cosmos_db_name]
    -> Database_user[$db_user]
    -> Database_grant[$db_user_and_db]
    -> anchor{'cosmos::master_db::end': }
}
