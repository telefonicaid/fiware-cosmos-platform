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

node 'cosmos-master' inherits default {
  include cosmos::master, firewall, cosmos::firewall::firewall_pre, cosmos::firewall::firewall_post
  Class['yum'] ~> Class['cosmos::master']

  resources { "firewall":
    purge => true
  }
  Class['cosmos::firewall::firewall_pre'] -> Firewall{} -> Class['cosmos::firewall::firewall_post']
}

node 'cosmos-store1' inherits default {
  class { 'cosmos::slave':
    ip          => "192.168.11.21",
    ct_ip       => "192.168.11.71",
    ct_hostname => "vagrant-store01",
  }
  Class['yum'] ~> Class['cosmos::slave']
}

node 'cosmos-store2' inherits default {
  class { 'cosmos::slave':
    ip          => "192.168.11.22",
    ct_ip       => "192.168.11.72",
    ct_hostname => "vagrant-store02",
  }
  Class['yum'] ~> Class['cosmos::slave']
}

node 'cosmos-compute1' inherits default {
  class { 'cosmos::slave':
    ip          => "192.168.11.23",
    ct_ip       => "192.168.11.73",
    ct_hostname => "vagrant-compute01",
  }
  Class['yum'] ~> Class['cosmos::slave']
}

node 'cosmos-compute2' inherits default {
  class { 'cosmos::slave':
    ip          => "192.168.11.24",
    ct_ip       => "192.168.11.74",
    ct_hostname => "vagrant-compute02",
  }
  Class['yum'] ~> Class['cosmos::slave']
}
