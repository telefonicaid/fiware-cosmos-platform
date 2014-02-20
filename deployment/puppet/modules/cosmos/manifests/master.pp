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

  include stdlib, ssh_keys, mysql, ambari::server,
    cosmos::base, cosmos::firewall::firewall_app, cosmos::apache::setup,
    cosmos::api, cosmos::localrepo

  anchor { 'cosmos::master::begin': }
    -> Class['stdlib', 'ssh_keys', 'mysql']
    -> Class['cosmos::base']
    -> Class['cosmos::localrepo']
    -> Class['ambari::server']
    -> Class['cosmos::api']
    -> Class['cosmos::apache::setup']
    -> Class['cosmos::firewall::firewall_app']
    -> anchor { 'cosmos::master::end': }

  file { '/root/.ssh/known_hosts':
    ensure  => 'present',
    content => template("${module_name}/known_hosts.erb"),
    group   => '0',
    mode    => '0644',
    owner   => '0',
  }
}
