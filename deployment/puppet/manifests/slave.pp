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

service {'iptables':
	ensure	=> stopped,
}

package {'java-1.7.0-openjdk':
  ensure => installed,
}

host { 'master':
  ip => '192.168.10.10',
  host_aliases => 'cosmos.master',	
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
