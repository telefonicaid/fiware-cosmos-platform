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
  $base_image_url  = 'http://cosmos10/develenv/repos/ovz-templates',
  $image_name = 'centos-6-cosmos.HDP.1.3.0-20131015-x86_64.tar.gz'
) {
  include ssh_keys, ambari::repos

  $image_url             = "${base_image_url}/${image_name}"
  $source_image_file_dir = '/tmp'
  $source_image_file     = "${source_image_file_dir}/${image_name}"
  $dest_image_file       = '/vz/template/cache/centos-6-cosmos-x86_64.tar.gz'
  $replacements_dir      = '/tmp/centos-6-cosmos-x86_64'

  wget::fetch { 'Download base image' :
    source      => $image_url,
    destination => $source_image_file,
  }

  exec { 'Remove extraction dir' :
    command => "rm -rf ${replacements_dir}",
    path    => '/bin',
  }

  file { 'Create extraction dir' :
    ensure => 'directory',
    path   => $replacements_dir,
  }

  exec { 'unpack_image' :
    command     => "tar -C ${replacements_dir} -zxf ${source_image_file}",
    user        => 'root',
    path        => '/bin',
    refreshonly => true
  }

  file { "${replacements_dir}/root/.ssh" :
    ensure  => 'directory',
    source  => '/root/.ssh',
    recurse => true,
    purge   => true,
    force   => true,
  }

  file { "${replacements_dir}/etc/ssh/ssh_host_rsa_key" :
    ensure  => 'present',
    source  => $cosmos::slave::ct_key_priv_file,
  }

  file { "${replacements_dir}/etc/ssh/ssh_host_rsa_key.pub" :
    ensure   => 'present',
    content  => $cosmos::slave::ct_key_pub,
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
    command     => "tar -C ${replacements_dir} -czf ${dest_image_file} .",
    user        => 'root',
    path        => '/bin',
    refreshonly => true
  }

  Class['ssh_keys'] ~> File["${replacements_dir}/root/.ssh"]
  Class['ambari::repos'] ~> File["${replacements_dir}/etc/yum.repos.d"]

  Wget::Fetch['Download base image']
    -> Exec['Remove extraction dir']
    ~> File['Create extraction dir']
    ~> Exec['unpack_image']
    ~> File["${replacements_dir}/root/.ssh",
            "${replacements_dir}/etc/yum.repos.d",
            "${replacements_dir}/etc/resolv.conf",
            "${replacements_dir}/etc/hosts",
            "${replacements_dir}/etc/sysconfig/network",
            "${replacements_dir}/etc/sysconfig/network-scripts/ifcfg-eth0",
            "${replacements_dir}/etc/ssh/ssh_host_rsa_key.pub",
            "${replacements_dir}/etc/ssh/ssh_host_rsa_key"]
    ~> Exec['pack_image']

  # Class 'ambari::repos' is not included here to avoid creating a cyclic dependency
  anchor {'cosmos::openvz::images::begin': }
    -> Class['ssh_keys']
    -> anchor {'cosmos::openvz::images::end': }
}
