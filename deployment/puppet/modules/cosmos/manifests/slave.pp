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
class cosmos::slave (
  $ip,
  $ct_ip,
  $ct_hostname,
  $ct_gateway,
  $netmask = $cosmos::params::cosmos_netmask,
  $gateway = '',
  $host_key_pub,
  $host_key_priv_file,
  $ct_key_pub,
  $ct_key_priv_file,
) inherits cosmos::params {
  include ssh_keys, cosmos::base, ambari::repos, cosmos::openvz::network,
      cosmos::openvz::images

  if member(hiera('slave_hosts'), $::hostname) == false {
    err("Host ${::hostname} is not listed in slave_hosts array in common.yaml.")
  }

  service { 'iptables':
    ensure  => stopped
  }

  class { 'cosmos::openvz::service':
    vz_utils_repo  => "${cosmos::params::cosmos_repo_deps_url}/OpenVZ/openvz-utils",
    vz_kernel_repo => "${cosmos::params::cosmos_repo_deps_url}/OpenVZ/openvz-kernel-rhel6",
    vz_repo_name   => 'cosmos-openvz',
  }

  class { 'libvirt':
    libvirt_repo_url => "${cosmos::params::cosmos_repo_deps_url}/libvirt",
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

  anchor {'cosmos::slave::begin': }
    -> Class['ambari::repos', 'cosmos::openvz::service', 'libvirt', 'cosmos::base']
    -> Class['ssh_keys', 'cosmos::openvz::network', 'cosmos::openvz::images']
    -> anchor {'cosmos::slave::end': }
}
