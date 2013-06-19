include pdi_base
include ambari::ambari_server
include mysql

service {'iptables':
	ensure	=> stopped,
}

package {'java-1.7.0-openjdk':
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

host { 'slave1':
  ip => '192.168.10.21',
  host_aliases => 'cosmos.slave1',
}

host { 'slave2':
  ip => '192.168.10.22',
  host_aliases => 'cosmos.slave2',
}

file { "/root/.ssh":
  ensure => "directory",
  mode   => 700,
}

file { "/root/.ssh/id_rsa":
  source  => "puppet:///modules/ambari/id_rsa",
  mode    => 600,
  owner   => root,
  group   => root,
  require => File["/root/.ssh"]
}

file { "/root/.ssh/id_rsa.pub":
  source => "puppet:///modules/ambari/id_rsa.pub",
  mode => 644,
  owner => root,
  group => root,
  require => File["/root/.ssh"]
}

ssh_authorized_key { "ssh_key":
  ensure => "present",
  key    => "AAAAB3NzaC1yc2EAAAADAQABAAABAQDKibr0qf5y/Iko/UfOq9npr8POkYfw6DYCfX4utzcp0tae9ZENpwu/ugDc3dZU6BMAo+T0diOxg8UQ77XXko/o9fzKA8WUtkmvosrUUEcfS/34XRHD0GiAdMSLt7BiAtlc4lJ8x/3S1lfWLlTe9f3+jY4mZKlLZnExvVWNFrtd0uxQdAj3JciisowYbUZpSId2GWdVuUdH+Y1y2y1JkTgtAnXt1lrCiH8WNOJZVkIhOXJM31OviAXGImSDk2JcycYTio81X/3xKua9yHJQ2AFZt5rh6u25s7VGxp85J5yijV9CV4oQDK51sxC8MIVFZ48YZVf2Ya4Bsfbk/AGtX+97",
  type   => "ssh-rsa",
  user   => "root"
}
