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

class cosmos::ssh_keys inherits cosmos::params {
  file { '/root/.ssh':
    ensure => 'directory',
    mode   => 700,
  }

  file { '/root/.ssh/id_rsa':
    content => $cosmos_private_key,
    mode    => 600,
    owner   => root,
    group   => root,
  }

  file { '/root/.ssh/id_rsa.pub':
    content => $cosmos_public_key,
    mode => 644,
    owner => root,
    group => root,
  }

  ssh_authorized_key { 'ssh_key':
    ensure => 'present',
    key    => $cosmos_raw_public_key,
    type   => 'ssh-rsa',
    user   => 'root'
  }

  File['/root/.ssh'] -> File['/root/.ssh/id_rsa', '/root/.ssh/id_rsa.pub']
  File['/root/.ssh'] -> Ssh_authorized_key['ssh_key']
}
