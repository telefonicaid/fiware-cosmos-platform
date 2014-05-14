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

  if $cosmos::params::master_use_ip_as_hostname {
    # This is a kinda of a hack: set master hostname as its IP. In this way
    # ambari server will set master hostname to be used by agents by its IP
    # instead of its hostname, which is not resoluble on slaves.
    class { 'hostname':
      hostname => $cosmos::params::master_ip,
      ip       => $cosmos::params::master_ip,
    }

    Class['hostname']
      -> Class['ambari::server']
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
