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

class cosmos::openvz::images {
  include cosmos::openvz::image_replacements

  wget::fetch { 'Download base image' :
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

  exec { 'Generate template' :
    command => '/tmp/generate-template.sh /tmp/centos-6-x86_64.tar.gz /tmp/centos-6-cosmos-x86_64.tar.gz',
    refreshonly => true,
    user => 'root',
  }

  file { '/tmp/template-configuration.properties' :
    ensure => 'present',
    content => template("${module_name}/template-configuration.properties.erb"),
    group   => '0',
    mode    => '644',
    owner   => '0',
  }

  exec { 'Configure template' :
    command => '/tmp/configure-template.sh /tmp/template-configuration.properties',
    refreshonly => true,
    user => 'root'
  }

  Wget::Fetch['Download base image'] ~> Exec['Generate template']
  File['/tmp/generate-template.sh'] ~> Exec['Generate template']
  Class['cosmos::openvz::image_replacements'] ~> Exec['Generate template']
    ~> Exec['Configure template']

  File['/tmp/functions.sh', '/tmp/configure-template.sh', '/tmp/template-configuration.properties']
    ~> Exec['Configure template']

  anchor {'cosmos::openvz::images::begin': }
    -> Class['cosmos::openvz::image_replacements']
    -> anchor {'cosmos::openvz::images::end': }
}