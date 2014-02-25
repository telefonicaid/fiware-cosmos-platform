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

  # This is a bit tricky: there are some directories (cosmos_confdir,
  # cosmos_stack_repo_path) that depends on it, but dependency is not implicit
  file { $cosmos::params::cosmos_basedir:
    ensure => 'directory',
  }

  file { $cosmos::params::cosmos_confdir:
    ensure => 'directory',
    mode   => '0440',
  }

  anchor { 'cosmos::master::begin': }
    -> Class['stdlib', 'ssh_keys', 'mysql']
    -> Class['cosmos::base']
    -> Class['cosmos::localrepo']
    -> Class['cosmos::apache::setup']
    -> Class['cosmos::firewall::firewall_app']
    -> Class['ambari::server']
    -> Class['cosmos::api']
    -> anchor { 'cosmos::master::end': }

  file { '/root/.ssh/known_hosts':
    ensure  => 'present',
    content => template("${module_name}/known_hosts.erb"),
    group   => '0',
    mode    => '0644',
    owner   => '0',
  }
}
