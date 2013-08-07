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
) {
  include cosmos::openvz::sysctl

  $vz_packages = [ 
      'vzkernel', 
      'vzctl', 
      'vzquota', 
      'vzstats', 
      'vzctl-core', 
      'vzkernel-firmware', 
      'vzkernel-headers']

  package { $vz_packages : 
    ensure    => installed,
  }

  service { 'vz' :
    ensure    => running,
    enable    => true,
  }

  file { 'vz.conf' :
    path      => "/etc/vz/vz.conf",
    ensure    => present,
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

  File['cosmos-openvz-repo']
    -> Package[$vz_packages]
    -> File['vz.conf']
    -> Class['openvz::sysctl']
    -> Service['vz']

  anchor {'cosmos::openvz::service::begin':}
    -> Class['cosmos::openvz::sysctl']
    -> anchor {'cosmos::openvz::service::end':}
}
