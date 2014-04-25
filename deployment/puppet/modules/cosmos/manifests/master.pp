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
