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
# Class: libvirt
#
# Install, enable and configure libvirt.
#
# Parameters:
#  $defaultnetwork:
#    Whether the default network for NAT should be enabled. Default: false
#  $virtinst:
#    Install the python-virtinst package, to get virt-install. Default: true
#  $qemu:
#    Install the qemu-kvm package, required for KVM. Default: true
#  $mdns_adv,
#  $unix_sock_group,
#  $unix_sock_ro_perms,
#  $unix_sock_rw_perms,
#  $unix_sock_dir:
#    Options for libvirtd.conf. Default: unchanged original values
#
# Sample Usage :
#  include libvirt
#
class libvirt (
    $libvirt_repo_url,
    $defaultnetwork     = undef,
    $virtinst           = true,
    $qemu               = undef,
    $openvz             = true,
    # libvirtd.conf options
    $mdns_adv           = '0',
    $unix_sock_group    = 'root',
    $unix_sock_ro_perms = '0777',
    $unix_sock_rw_perms = '0700',
    $unix_sock_dir      = '/var/run/libvirt',
    $listen_tls         = '1',
    $listen_tcp         = '1',
    $package_ensure     = 'installed',
    # Service related
    $svc_enable         = true,
    $svc_ensure         = 'running',
) {

    file { 'cosmos-libvirt.repo' :
        ensure  => present,
        owner   => 'root',
        mode    => '0644',
        path    => '/etc/yum.repos.d/cosmos-libvirt.repo',
        content => template('libvirt/libvirt-repo.erb'),
        #notify  => Service['libvirtd'],
        #require => Package['libvirt'],
    }

    #yumrepo { "cosmos-libvirt":
    #  baseurl => "http://cosmos10.hi.inet/develenv/rpms/cosmos-deps/libvirt/",
    #  descr => "Cosmos LibVirt with OpenVZ - v1.0.5 - NO PolKIT",
    #  enabled => 1,
    #  gpgcheck => 0,
    #  name => "cosmos-libvirt",
    #}

    package { 'libvirt':
      ensure => $package_ensure,
    }

    file { 'libvirt.conf' :
        path    => '/etc/libvirt/libvirtd.conf',
        content => template('libvirt/libvirtd.conf.erb'),
        notify  => Service['libvirtd'],
        #require => Package['libvirt'],
    }

    service { 'libvirtd':
      ensure    => $svc_ensure,
      enable    => $svc_enable,
      hasstatus => true,
      require   => Package['libvirt'],
    }

    # Not needed until we support changes to it
    # file { '/etc/libvirt/qemu.conf':
    #    content => template('libvirt/qemu.conf.erb'),
    #    notify  => Service['libvirtd'],
    #    require => Package['libvirt'],
    #}

    # The default network, automatically configured... disable it by default
    if $defaultnetwork {
        file { '/etc/libvirt/qemu/networks/autostart/default.xml':
            ensure  => link,
            target  => '../default.xml',
            require => Package['libvirt'],
        }
    } else {
        file { '/etc/libvirt/qemu/networks/autostart/default.xml':
            ensure  => absent,
            require => Package['libvirt'],
        }
    }

    # The most useful libvirt-related packages
    if $virtinst {
        package { 'python-virtinst' :
          ensure => installed,
        }
        File['cosmos-libvirt.repo'] -> Package['python-virtinst']
    }
    if $qemu {
        package { 'qemu-kvm' :
          ensure => installed,
        }
    }

  File['cosmos-libvirt.repo']
    -> Package['libvirt']
    -> File['libvirt.conf']
    -> Service['libvirtd']
}

