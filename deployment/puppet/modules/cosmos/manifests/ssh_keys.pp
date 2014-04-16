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

class cosmos::ssh_keys inherits cosmos::params {
  file { '/root/.ssh':
    ensure => 'directory',
    mode   => '0700',
  }

  file { '/root/.ssh/id_rsa':
    content => $cosmos::params::cosmos_private_key,
    mode    => '0600',
    owner   => root,
    group   => root,
  }

  file { '/root/.ssh/id_rsa.pub':
    content => $cosmos::params::cosmos_public_key,
    mode    => '0644',
    owner   => root,
    group   => root,
  }

  ssh_authorized_key { 'ssh_key':
    ensure => 'present',
    key    => $cosmos::params::cosmos_raw_public_key,
    type   => 'ssh-rsa',
    user   => 'root'
  }

  File['/root/.ssh'] -> File['/root/.ssh/id_rsa', '/root/.ssh/id_rsa.pub']
  File['/root/.ssh'] -> Ssh_authorized_key['ssh_key']
}
