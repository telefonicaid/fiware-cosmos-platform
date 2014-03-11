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

class cosmos::openvz::service  (
    $vz_utils_repo,
    $vz_kernel_repo,
    $vz_repo_name,
    $vz_kernel_version = $cosmos::params::openvz_vzkernel_version,
    $vz_ctl_ensure     = $cosmos::params::openvz_vzctl_version,
    $vz_quota_ensure   = $cosmos::params::openvz_vzquota_version,
    $vz_stats_ensure   = $cosmos::params::openvz_vzstats_version
) {
  include cosmos::openvz::sysctl, stdlib

  $vz_ctl_packages    = ['vzctl', 'vzctl-core']

  exec { 'check kernel release' :
    command => "/bin/bash -c \"[[ \"`uname -r`\" == \"${vz_kernel_version}\" ]]\"",
    returns => 0
  }

  package { $vz_ctl_packages :
    ensure    => $vz_ctl_ensure,
  }

  package { 'vzquota' :
    ensure    => $vz_quota_ensure,
  }

  package { 'vzstats' :
    ensure    => $vz_stats_ensure,
  }

  service { 'vz' :
    ensure    => running,
    enable    => true,
  }

  file { 'vz.conf' :
    ensure    => present,
    path      => '/etc/vz/vz.conf',
    source    => "puppet:///modules/${module_name}/vz.conf",
    notify    => Service['vz'],
  }

  file { "${vz_repo_name}-repo" :
    ensure    => present,
    owner     => 'root',
    group     => 'root',
    mode      => '0644',
    path      => "/etc/yum.repos.d/${vz_repo_name}.repo",
    content   => template("${module_name}/openvz.repo.erb"),
  }

  File["${vz_repo_name}-repo"]
    -> Exec ['check kernel release']
    -> Package[$vz_ctl_packages]
    -> File['vz.conf']
    -> Class['openvz::sysctl']
    ~> Service['vz']

  anchor {'cosmos::openvz::service::begin':}
    -> Class['cosmos::openvz::sysctl']
    -> anchor {'cosmos::openvz::service::end':}
}
