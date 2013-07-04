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

class cosmos::openvz_images {
  wget::fetch { 'download base image' :
    source => "http://cosmos10/develenv/repos/ovz-templates/centos-6-x86_64.tar.gz",
    destination => "/tmp/centos-6-x86_64.tar.gz",
  }

  file { '/tmp/generate-template.sh' :
    ensure => 'present',
    source => "puppet:///modules/${module_name}/generate-template.sh",
  }

  file { '/tmp/functions.sh' :
    ensure => 'present',
    source => "puppet:///modules/${module_name}/functions.sh",
  }

  file { '/tmp/configure-template.sh' :
    ensure => 'present',
    source => "puppet:///modules/${module_name}/configure-template.sh",
  }

  # Empty directory. FIXME!
  file { [ '/tmp/replacements', '/tmp/replacements/centos-6-x86_64.tar.gz' ] :
    ensure => 'directory',
  }

  file { '/tmp/replacements/centos-6-x86_64.tar.gz/dummy' :
    ensure => 'present',
    require => File['/tmp/replacements/centos-6-x86_64.tar.gz'],
  }

  exec { '/tmp/generate-template.sh /tmp/centos-6-x86_64.tar.gz /tmp/centos-6-cosmos-x86_64.tar.gz' :
    creates => '/tmp/centos-6-cosmos-x86_64.tar.gz',
    user => 'root',
    require => [ Wget::Fetch['download base image'], File['/tmp/generate-template.sh'], File['/tmp/replacements/centos-6-x86_64.tar.gz/dummy'] ],
  }

  file { '/tmp/template-configuration.properties' :
    ensure => 'present',
    content => template("${module_name}/template-configuration.properties.erb"),
    group   => '0',
    mode    => '644',
    owner   => '0',
  }

  exec { '/tmp/configure-template.sh /tmp/template-configuration.properties' :
    creates => '/vz/template/cache/centos-6-cosmos-x86_64.tar.gz',
    user => 'root',
    require => [ File['/tmp/template-configuration.properties'], Exec['/tmp/generate-template.sh /tmp/centos-6-x86_64.tar.gz /tmp/centos-6-cosmos-x86_64.tar.gz'] ],
  }
}