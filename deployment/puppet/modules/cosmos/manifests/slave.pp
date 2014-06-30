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
class cosmos::slave (
  $ip,
  $ct_ip,
  $ct_hostname,
  $ct_hostname_is_nat_ip = false,
  $ct_gateway,
  $ct_routes = '',
  $netmask = $cosmos::params::cosmos_netmask,
  $gateway = '',
  $host_key_pub,
  $host_key_priv_file,
  $ct_key_pub,
  $ct_key_priv_file,
) inherits cosmos::params {
  include ssh_keys, cosmos::base, cosmos::openvz::network,
    cosmos::openvz::images, ambari::params

  if member(hiera('slave_hosts'), $::hostname) == false {
    err("Host ${::hostname} is not listed in slave_hosts array in common.yaml.")
  }

  service { 'iptables':
    ensure  => running
  }

  file { '/etc/sysconfig/iptables-config':
    ensure => present,
    source => "puppet:///modules/${module_name}/iptables-config",
    owner  => root,
    group  => root
  }

  file { '/etc/sysconfig/iptables':
    ensure => present,
    source => "puppet:///modules/${module_name}/iptables",
    owner  => root,
    group  => root
  }

  class { 'cosmos::openvz::service':
    vz_utils_repo  => "${cosmos::params::cosmos_repo_deps_url}/OpenVZ/openvz-utils",
    vz_kernel_repo => "${cosmos::params::cosmos_repo_deps_url}/OpenVZ/openvz-kernel-rhel6",
    vz_repo_name   => 'cosmos-openvz',
  }

  class { 'libvirt':
    libvirt_repo_url => "${cosmos::params::cosmos_repo_deps_url}/libvirt",
    package_ensure   => $cosmos::params::libvirt_version,
    svc_enable       => true,
    svc_ensure       => 'running',
  }

  file { '/etc/ssh/ssh_host_rsa_key':
    ensure => 'present',
    source => $host_key_priv_file,
    mode   => '0600',
    owner  => root,
    group  => root,
  }

  file { '/etc/ssh/ssh_host_rsa_key.pub':
    ensure  => 'present',
    content => $host_key_pub,
    mode    => '0644',
    owner   => root,
    group   => root,
  }

  exec { 'Update CT Ambari Agent':
    command => 'vzctl exec 101 "ambari-agent stop && yum clean all && yum update -y ambari-agent && ambari-agent start"',
    path    => ['/usr/sbin/', '/bin/'],
    onlyif  => 'vzctl status 101 | grep running',
  }

  exec { 'Cleanup repos':
    command => 'vzctl exec 101 "rm -f /etc/yum.repos.d/HDP.repo && rm -f /var/lib/rpm/__db.00* && rpm --rebuilddb && yum clean all && yum history new"',
    path    => ['/usr/sbin/', '/bin/'],
    onlyif  => 'vzctl status 101 | grep running',
  }

  if $ambari::params::enable_repo_mirroring {
    exec { 'Disable cosmos-platform repo':
      command => 'vzctl exec 101 "mv /etc/yum.repos.d/cosmos-platform.repo /etc/yum.repos.d/cosmos-plaform.repo.disabled || true"',
      path    => ['/usr/sbin/', '/bin/'],
      onlyif  => 'vzctl status 101 | grep running',
    }
    Exec['Update CT Ambari Agent']
      -> Exec['Disable cosmos-platform repo']
      -> Exec['Cleanup repos']
  } else {
    exec { 'Enable cosmos-platform repo':
      command => 'vzctl exec 101 "mv /etc/yum.repos.d/cosmos-platform.repo.disabled /etc/yum.repos.d/cosmos-plaform.repo || true"',
      path    => ['/usr/sbin/', '/bin/'],
      onlyif  => 'vzctl status 101 | grep running',
    }
    Exec['Enable cosmos-platform repo']
      -> Exec['Cleanup repos']
      -> Exec['Update CT Ambari Agent']
  }

  File['/etc/sysconfig/iptables-config', '/etc/sysconfig/iptables']
    ~> Service['iptables']
    -> Class['cosmos::openvz::service', 'cosmos::openvz::network']
    -> Exec['Update CT Ambari Agent']

  anchor {'cosmos::slave::begin': }
    -> Class['cosmos::openvz::service', 'libvirt', 'cosmos::base', 'ambari::params']
    -> Class['ssh_keys', 'cosmos::openvz::network', 'cosmos::openvz::images']
    -> anchor {'cosmos::slave::end': }
}
