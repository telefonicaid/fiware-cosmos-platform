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
  $netmask,
  $host_key_pub,
  $host_key_pub_file
) inherits cosmos::params {
  include ssh_keys, cosmos::base, cosmos::openvz::network

  class {'cosmos::openvz::images':
    gateway => $ip,
  }

  if member(hiera('slave_hosts'), $hostname) == false {
    err("Host ${hostname} is not listed in slave_hosts array in common.yaml.")
  }

  service { 'iptables':
    ensure	=> stopped
  }

  class { 'openvz':
    vz_utils_repo   => "${cosmos_repo_url}/cosmos-deps/OpenVZ/openvz-utils",
    vz_kernel_repo  => "${cosmos_repo_url}/cosmos-deps/OpenVZ/openvz-kernel-rhel6",
  }

  class { 'libvirt':
    libvirt_repo_url => "${cosmos_repo_url}/cosmos-deps/libvirt",
    svc_enable       => "true",
    svc_ensure       => "running",
  }

  file { '/etc/ssh/ssh_host_rsa_key':
    ensure => "present",
    source => $host_key_pub_file,
    mode => 600,
    owner => root,
    group => root,
  }

  file { '/etc/ssh/ssh_host_rsa_key.pub':
    ensure => "present",
    content => $host_key_pub,
    mode => 644,
    owner => root,
    group => root,
  }

  anchor {'cosmos::slave::begin': }
    -> Class['openvz', 'libvirt', 'cosmos::base']
    -> Class['ssh_keys', 'cosmos::openvz::network', 'cosmos::openvz::images']
    -> anchor {'cosmos::slave::end': }
}
