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

class cosmos::openvz::images(
  $ip_address = $cosmos::slave::ct_ip,
  $netmask    = $cosmos::slave::netmask,
  $gateway    = $cosmos::slave::ct_gateway, 
  $image_url  = 'http://cosmos10/develenv/repos/ovz-templates/centos-6-x86_64.tar.gz',
) {
  include ssh_keys, ambari_repos

  $source_image_file    = '/tmp/centos-6-x86_64.tar.gz'
  $dest_image_file      = '/vz/template/cache/centos-6-cosmos-x86_64.tar.gz'
  $replacements_dir     = '/tmp/centos-6-cosmos-x86_64'

  wget::fetch { 'Download base image' :
    source      => $image_url,
    destination => $source_image_file,
  }

  file { $replacements_dir :
    ensure => 'directory',
  }

  exec { 'unpack_image' :
    command => "tar -C ${replacements_dir} -zxf ${source_image_file}",
    user    => 'root',
    path    => '/bin'
  }

  file { "${replacements_dir}/root/.ssh" :
    ensure  => 'directory',
    source  => '/root/.ssh',
    recurse => true,
    purge   => true,
    force   => true,
  }

  file { "${replacements_dir}/etc/yum.repos.d" :
    ensure   => 'directory',
    source   => '/etc/yum.repos.d',
    recurse  => true,
    purge    => true,
    force    => true,
  }

  file { "${replacements_dir}/etc/resolv.conf" :
    ensure => 'present',
    source => '/etc/resolv.conf',
  }

  file { "${replacements_dir}/etc/hosts" :
    ensure => 'present',
    source => '/etc/hosts',
  }

  file { "${replacements_dir}/etc/sysconfig/network" :
    ensure  => 'present',
    content => template("${module_name}/network.erb"),
  }

  file { "${replacements_dir}/etc/sysconfig/network-scripts/ifcfg-eth0" :
    ensure  => 'present',
    content => template("${module_name}/ifcfg-eth0.erb"),
  }

  exec { 'pack_image' :
    command => "tar -C ${replacements_dir} -czf ${dest_image_file} .",
    user    => 'root',
    path    => '/bin'
  }

  # Due to having different adapters in Vagrant for internet access and host access
  # we need to route internet traffic through eth0 explicitly
  if $environment == 'vagrant' {
    exec { '/sbin/iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE':
      user => 'root',
    }
  }

  Class['ssh_keys'] ~> File["${replacements_dir}/root/.ssh"]
  Class['ambari_repos'] ~> File["${replacements_dir}/etc/yum.repos.d"]

  Wget::Fetch['Download base image'] 
    ~> File[$replacements_dir]
    ~> Exec['unpack_image']
    ~> File["${replacements_dir}/root/.ssh", 
            "${replacements_dir}/etc/yum.repos.d", 
            "${replacements_dir}/etc/resolv.conf", 
            "${replacements_dir}/etc/hosts",
            "${replacements_dir}/etc/sysconfig/network",
            "${replacements_dir}/etc/sysconfig/network-scripts/ifcfg-eth0"]
    ~> Exec['pack_image']

  # Class 'ambari_repos' is not included here to avoid creating a cyclic dependency
  anchor {'cosmos::openvz::images::begin': }
    -> Class['ssh_keys']
    -> anchor {'cosmos::openvz::images::end': }
}
