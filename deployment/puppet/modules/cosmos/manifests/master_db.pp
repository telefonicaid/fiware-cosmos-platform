#
# Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

class cosmos::master_db inherits cosmos::params {
  class { 'mysql::server':
    config_hash => { 'root_password' => 'cosmos' },
  }

  mysql::server::config { 'basic_config':
    settings => {
      'client' => {
        'default-character-set'  => 'utf8'
      },
      'mysqld' => {
        'bind-address'           => '127.0.0.1',
        'default-storage-engine' => 'INNODB',
        'collation-server'       => 'utf8_unicode_ci',
        'init-connect'           => "'SET NAMES utf8'",
        'character-set-server'   => 'utf8'
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
