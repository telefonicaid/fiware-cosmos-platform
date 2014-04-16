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

class cosmos::base inherits cosmos::params {
  include cosmos::cluster_hosts, timezone
  
  class { '::ntp':
    servers => [ $cosmos::params::ntp_server ],
  }

  package { 'java-1.7.0-openjdk':
    ensure => installed,
  }

  file { '/etc/puppet':
    ensure => directory,
    owner  => 'root',
    group  => 'root'
  }

  if $environment == 'vagrant' {
    file { '/etc/puppet/puppet.conf':
      source  => "puppet:///modules/${module_name}/puppet.conf",
      owner   => 'root',
      group   => 'root',
    }
    File['/etc/puppet'] -> File['/etc/puppet/puppet.conf']
  }

  anchor { 'cosmos::base::begin': }
    -> Class['cosmos::cluster_hosts', '::ntp', 'timezone']
    -> anchor { 'cosmos::base::end': }
}
