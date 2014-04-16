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

class cosmos::firewall::firewall_app(
  $subnet  = $cosmos::params::cosmos_subnet,
  $netmask = $cosmos::params::cosmos_netmask
) inherits cosmos::params {
  firewall { '100 allow Ambari API and master RPM repo ports access from localhost':
    dport  => [8080, $cosmos::params::master_repo_port],
    proto  => tcp,
    action => accept,
    source => '127.0.0.1',
  }

  firewall { '101 allow Ambari API and master RPM repo ports access from cluster nodes subnet':
    dport  => [8080, $cosmos::params::master_repo_port],
    proto  => tcp,
    action => accept,
    source => "${subnet}/${netmask}",
  }

  firewall { '102 deny Ambari API and master RPM repo ports access from outside':
    dport  => [8080, $cosmos::params::master_repo_port],
    proto  => tcp,
    action => drop,
  }

  Firewall['100 allow Ambari API and master RPM repo ports access from localhost']
    -> Firewall['101 allow Ambari API and master RPM repo ports access from cluster nodes subnet']
    -> Firewall['102 deny Ambari API and master RPM repo ports access from outside']

  firewall { '103 allow 9000 access from localhost':
    dport  => 9000,
    proto  => tcp,
    action => accept,
    source => '127.0.0.1',
  }

  firewall { '104 deny 9000 access from outside':
    dport  => 9000,
    proto  => tcp,
    action => drop,
  }

  Firewall['103 allow 9000 access from localhost']
    -> Firewall['104 deny 9000 access from outside']
}
