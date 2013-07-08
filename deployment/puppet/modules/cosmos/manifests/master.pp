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

  include stdlib, ssh_keys, mysql, ambari,
    cosmos::base, cosmos::firewall::firewall_app, cosmos::master_setup, cosmos::api

  anchor { 'cosmos::master::begin': }
  anchor { 'cosmos::master::end': }

  Anchor ['cosmos::master::begin']
  ->
  Class['stdlib', 'ssh_keys']
  ->
  Class ['cosmos::base']
  ->
  Class ['ambari', 'mysql']
  ->
  Class ['cosmos::master_setup'] -> Class ['cosmos::api']
  ->
  Class ['cosmos::firewall::firewall_app']
  ->
  Anchor ['cosmos::master::end']

  file { '/root/.ssh/known_hosts':
    ensure => 'present',
    content => template("${module_name}/known_hosts.erb"),
    group   => '0',
    mode    => '644',
    owner   => '0',
  }
}
