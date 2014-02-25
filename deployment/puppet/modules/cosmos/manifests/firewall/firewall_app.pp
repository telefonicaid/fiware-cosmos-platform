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
