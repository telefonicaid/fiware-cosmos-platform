#
# Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

class cosmos::openvz::images(
  $ip_address = $cosmos::slave::ct_ip,
  $netmask    = $cosmos::slave::netmask,
  $gateway    = $cosmos::slave::ct_gateway,
  $routes     = $cosmos::slave::ct_routes,
  $ct_hostname = $cosmos::slave::ct_hostname,
  $natted     = $cosmos::slave::ct_hostname_is_nat_ip,
  $base_image_url,
  $image_name,
  $source_image_file_dir = '/tmp'
) {
  include ssh_keys

  $image_url             = "${base_image_url}/${image_name}"
  $source_image_file     = "${source_image_file_dir}/${image_name}"
  $dest_image_file       = '/vz/template/cache/centos-6-cosmos-x86_64.tar.gz'
  $replacements_dir      = '/tmp/centos-6-cosmos-x86_64'

  if $cosmos::params::proxy {
    $environment = [ "HTTP_PROXY=${cosmos::params::proxy}", "http_proxy=${cosmos::params::proxy}" ]
  } else {
    $environment = []
  }

  exec { "Download base image":
    command     => "wget --output-document=${$source_image_file} ${image_url}",
    environment => $environment,
    path        => '/usr/bin:/usr/sbin:/bin:/usr/local/bin:/opt/local/bin',
    timeout     => 900,
    creates     => $source_image_file,
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
    path        => '/bin'
  }

  file { "${replacements_dir}/root/.ssh" :
    ensure  => 'directory',
    source  => '/root/.ssh',
    recurse => true,
    purge   => true,
    force   => true,
  }

  file { "${replacements_dir}/root/.ssh/id_rsa" :
    ensure  => absent,
  }

  file { "${replacements_dir}/root/.ssh/id_rsa.pub" :
    ensure  => absent,
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

  if !empty('routes') {
    file { "${replacements_dir}/etc/sysconfig/network-scripts/route-eth0" :
      ensure  => 'present',
      content => $routes,
    }

    File["${replacements_dir}/etc/sysconfig/network-scripts/route-eth0"]
    -> Exec['pack_image']

  }

  if $natted {
    file { "${replacements_dir}/etc/sysconfig/network-scripts/ifcfg-eth0:0" :
      ensure  => 'present',
      content => template("${module_name}/ifcfg-eth0_0.erb"),
    }

    file { "${replacements_dir}/etc/sysconfig/iptables" :
      ensure  => 'present',
      content => template("${module_name}/iptables-nat.erb"),
    }

    File["${replacements_dir}/etc/sysconfig/network-scripts/ifcfg-eth0:0",
         "${replacements_dir}/etc/sysconfig/iptables"]
    -> Exec['pack_image']
  }

  file { "${replacements_dir}/etc/localtime" :
    ensure  => 'present',
    source => '/etc/localtime'
  }

  exec { 'pack_image' :
    command     => "tar -C ${replacements_dir} -czf ${dest_image_file} .",
    user        => 'root',
    path        => '/bin',
    timeout     => 600
  }

  Class['ssh_keys'] ~> File["${replacements_dir}/root/.ssh"]

  Exec['Download base image']
    -> Exec['Remove extraction dir']
    ~> File['Create extraction dir']
    -> Exec['unpack_image']
    ~> File["${replacements_dir}/root/.ssh",
            "${replacements_dir}/etc/yum.repos.d",
            "${replacements_dir}/etc/resolv.conf",
            "${replacements_dir}/etc/hosts",
            "${replacements_dir}/etc/sysconfig/network",
            "${replacements_dir}/etc/sysconfig/network-scripts/ifcfg-eth0",
            "${replacements_dir}/etc/ssh/ssh_host_rsa_key.pub",
            "${replacements_dir}/etc/ssh/ssh_host_rsa_key",
            "${replacements_dir}/etc/localtime"]
    ~> File["${replacements_dir}/root/.ssh/id_rsa", "${replacements_dir}/root/.ssh/id_rsa.pub"]
    -> Exec['pack_image']

  anchor {'cosmos::openvz::images::begin': }
    -> Class['ssh_keys']
    -> anchor {'cosmos::openvz::images::end': }
}
