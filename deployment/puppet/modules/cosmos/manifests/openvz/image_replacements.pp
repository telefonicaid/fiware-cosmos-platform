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

class cosmos::openvz::image_replacements inherits cosmos::params {
  include ssh_keys

  file { [
      $cosmos::params::openvz_rplcements_dir,
      $cosmos::params::openvz_targz_path,
      "${cosmos::params::openvz_targz_path}/etc",
      "${cosmos::params::openvz_targz_path}/root" ] :
    ensure => 'directory',
  }

  file { "${cosmos::params::openvz_targz_path}/root/.ssh" :
    ensure  => 'directory',
    source  => '/root/.ssh',
    recurse => true,
    purge   => true,
    force   => true,
  }

  file { "${cosmos::params::openvz_targz_path}/etc/yum.repos.d" :
    ensure   => 'directory',
    source   => '/etc/yum.repos.d',
    recurse  => true,
    purge    => true,
    force    => true,
  }

  file { "${cosmos::params::openvz_targz_path}/etc/resolv.conf" :
    ensure => 'present',
    source => '/etc/resolv.conf',
  }

  file { "${cosmos::params::openvz_targz_path}/etc/hosts" :
    ensure => 'present',
    source => '/etc/hosts',
  }

  File["${cosmos::params::openvz_targz_path}/root"] -> File["${cosmos::params::openvz_targz_path}/root/.ssh"]
  File["${cosmos::params::openvz_targz_path}/etc"]  -> File[
      "${cosmos::params::openvz_targz_path}/etc/resolv.conf",
      "${cosmos::params::openvz_targz_path}/etc/yum.repos.d",
      "${cosmos::params::openvz_targz_path}/etc/hosts"]

  Class['ssh_keys']     ~> File["${cosmos::params::openvz_targz_path}/root/.ssh"]
  Class['ambari_repos'] ~> File[ "${cosmos::params::openvz_targz_path}/etc/yum.repos.d"]

  anchor { 'cosmos::openvz::image_replacements::begin': }
    -> Class['ssh_keys']
    -> anchor { 'cosmos::openvz::image_replacements::end': }
}
