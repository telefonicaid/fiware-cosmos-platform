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

class cosmos::openvz_network {
	service { 'network' :
    ensure => "running",
    enable => "true",
  }

  file { '/etc/sysconfig/network-scripts/ifcfg-vzbr0' :
    ensure => 'present',
    content => template("${module_name}/ifcfg-vzbr0.erb"),
    group   => '0',
    mode    => '644',
    owner   => '0',
    notify  => Service['network'],
  }

  file { '/etc/vz/conf/ve-g1-compute.conf-sample' :
    ensure => 'present',
    source => "puppet:///modules/${module_name}/ve-g1-compute.conf-sample",
    group   => '0',
    mode    => '644',
    owner   => '0',
  }

  editfile::config { 'add_bridge' :
    path => '/etc/sysconfig/network-scripts/ifcfg-eth1',
    entry => 'BRIDGE',
    ensure => 'vzbr0',
  }

  editfile::config { 'remove_ip_config' :
    ensure => 'absent',
    path => '/etc/sysconfig/network-scripts/ifcfg-eth1',
    entry => 'IPADDR',
    notify  => Service['network'],
  }

  editfile::config { 'remove_netmask' :
    ensure => 'absent',
    path => '/etc/sysconfig/network-scripts/ifcfg-eth1',
    entry => 'NETMASK',
    notify  => Service['network'],
  }

  file { '/etc/vz/vznet.conf' :
    ensure => 'present',
    content => 'EXTERNAL_SCRIPT="/usr/sbin/vznetaddbr"',
    notify  => Service['network'],
  }
}