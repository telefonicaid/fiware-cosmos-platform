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
class cosmos::slave {
  include pdi_base, ssh_keys, cosmos::params, cosmos::cluster_hosts

  service {'iptables':
    ensure	=> stopped,
  }

  package {'java-1.7.0-openjdk':
    ensure => installed,
  }

  class { 'openvz' :
    vz_utils_repo   => "${cosmos::params::cosmos_repo_url}/OpenVZ/openvz-utils",
    vz_kernel_repo  => "${cosmos::params::cosmos_repo_url}/OpenVZ/openvz-kernel-rhel6",
  }

  class { 'libvirt' :
    libvirt_repo_url => "${cosmos::params::cosmos_repo_url}/libvirt",
    svc_enable       => "true",
    svc_ensure       => "running",
  }
}
