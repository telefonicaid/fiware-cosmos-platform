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
  $netmask
) inherits cosmos::base {
  include ssh_keys
  include cosmos::openvz::network
  include cosmos::openvz::images

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

  anchor {'cosmos::slave::begin': }
  ->
  Class['openvz', 'libvirt', 'ssh_keys', 'cosmos::openvz::network', 'cosmos::openvz::images']
  ->
  anchor {'cosmos::slave::end': }
}
