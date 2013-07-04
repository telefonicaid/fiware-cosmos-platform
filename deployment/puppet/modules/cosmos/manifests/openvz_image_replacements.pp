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

class cosmos::openvz_image_replacements {
  include ssh_keys

  file { [
      '/tmp/replacements',
      '/tmp/replacements/centos-6-x86_64.tar.gz',
      '/tmp/replacements/centos-6-x86_64.tar.gz/root/' ] :
    ensure => 'directory',
  }

  file { '/tmp/replacements/centos-6-x86_64.tar.gz/root/.ssh' :
    ensure => 'directory',
    source => '/root/.ssh',
    recurse => true,
    purge => true,
    force => true,
    require => File['/tmp/replacements/centos-6-x86_64.tar.gz/root'],
    subscribe => Class['ssh_keys'],
  }
}