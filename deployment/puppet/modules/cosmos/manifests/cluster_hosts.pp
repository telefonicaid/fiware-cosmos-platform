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

class cosmos::cluster_hosts inherits cosmos::params {

  # NOTE: This function assumes that ct_hostname contains the vm public IP.
  define addHostEntry {
    host { $title:
      ip => hiera('cosmos::slave::ct_hostname',nil, $title),
    }
  }

  if $overwrite_hosts_file {
    file { '/etc/hosts' :
      ensure  => 'present',
      content => template("${module_name}/hosts.erb"),
      group   => '0',
      mode    => '0644',
      owner   => '0',
    }
  } else {
    $hosts = hiera('slave_hosts')
    addHostEntry{ $hosts: }
  }
}
