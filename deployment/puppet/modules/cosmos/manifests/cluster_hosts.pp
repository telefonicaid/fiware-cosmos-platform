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

class cosmos::cluster_hosts inherits cosmos::params {

  # NOTE: This function presupones that ct_hostname contains the vm public IP.
  define addHostEntry {
    host { $title:
      ip => hiera('cosmos::slave::ct_hostname',nil, $title),
    }
  }

  if $overwrite_hosts_file {
    file { '/etc/hosts' :
      ensure  => 'present',
      content => template("${module_name}/hosts.erb"),
      group   => '0',
      mode    => '0644',
      owner   => '0',
    }
  } else {
    $hosts = hiera('slave_hosts')
    addHostEntry{ $hosts: }
  }
}
