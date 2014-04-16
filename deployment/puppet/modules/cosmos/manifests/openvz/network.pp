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

class cosmos::openvz::network($host_iface) {
  service { 'network' :
    ensure => 'running',
    enable => true,
  }

  file { '/etc/sysconfig/network-scripts/ifcfg-vzbr0' :
    ensure  => 'present',
    content => template("${module_name}/ifcfg-vzbr0.erb"),
    group   => '0',
    mode    => '0644',
    owner   => '0',
  }

  file { '/etc/vz/conf/ve-g1-compute.conf-sample' :
    ensure  => 'present',
    source  => "puppet:///modules/${module_name}/ve-g1-compute.conf-sample",
    group   => '0',
    mode    => '0644',
    owner   => '0',
  }

  editfile::config { 'add_bridge' :
    ensure => 'vzbr0',
    path   => "/etc/sysconfig/network-scripts/ifcfg-${host_iface}",
    entry  => 'BRIDGE',
  }

  editfile::config { 'remove_ip_config' :
    ensure => 'absent',
    path   => "/etc/sysconfig/network-scripts/ifcfg-${host_iface}",
    entry  => 'IPADDR',
  }

  editfile::config { 'remove_netmask' :
    ensure => 'absent',
    path   => "/etc/sysconfig/network-scripts/ifcfg-${host_iface}",
    entry  => 'NETMASK',
  }

  file { '/etc/vz/vznet.conf' :
    ensure  => 'present',
    content => 'EXTERNAL_SCRIPT="/usr/sbin/vznetaddbr"',
  }

  # Due to having different adapters in Vagrant for internet access and host access
  # we need to route internet traffic through eth0 explicitly
  if $environment == 'vagrant' {
    exec { '/sbin/iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE':
      user => 'root',
    }
    exec { '/sbin/service iptables save':
      user => 'root',
    }
    Exec['/sbin/iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE']
      -> Exec['/sbin/service iptables save']
  }

  File['/etc/sysconfig/network-scripts/ifcfg-vzbr0', '/etc/vz/vznet.conf']
    ~> Service['network']
  Editfile::Config['remove_ip_config', 'remove_netmask', 'add_bridge']
    ~> Service['network']
}
