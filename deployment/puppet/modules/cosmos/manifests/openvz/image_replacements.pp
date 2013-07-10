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
  include ssh_keys, ambari_repos

  file { [
      $openvz_rplcements_dir,
      $openvz_targz_path,
      "${openvz_targz_path}/etc",
      "${openvz_targz_path}/root" ] :
    ensure => 'directory',
  }

  file { "${openvz_targz_path}/root/.ssh" :
    ensure  => 'directory',
    source  => '/root/.ssh',
    recurse => true,
    purge   => true,
    force   => true,
  }

  file { "${openvz_targz_path}/etc/yum.repos.d" :
    ensure   => 'directory',
    source   => '/etc/yum.repos.d',
    recurse  => true,
    purge    => true,
    force    => true,
  }

  file { "${openvz_targz_path}/etc/resolv.conf" :
    ensure => 'present',
    source => '/etc/resolv.conf',
  }

  file { "${openvz_targz_path}/etc/hosts" :
    ensure => 'present',
    source => '/etc/hosts',
  }

  File["${openvz_targz_path}/root"] -> File["${openvz_targz_path}/root/.ssh"]
  File["${openvz_targz_path}/etc"]  -> File[
      "${openvz_targz_path}/etc/resolv.conf",
      "${openvz_targz_path}/etc/yum.repos.d",
      "${openvz_targz_path}/etc/hosts"]

  Class['ssh_keys']     ~> File["${openvz_targz_path}/root/.ssh"]
  Class['ambari_repos'] ~> File[ "${openvz_targz_path}/etc/yum.repos.d"]

  anchor { 'cosmos::openvz::image_replacements::begin': }
    -> Class['ssh_keys', 'ambari_repos']
    -> anchor { 'cosmos::openvz::image_replacements::end': }
}
