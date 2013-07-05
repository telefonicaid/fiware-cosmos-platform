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

node 'cosmos-store1', 'cosmos-store2', 'cosmos-compute1', 'cosmos-compute2' inherits default {
  include cosmos::slave
  Class['yum'] ~> Class['cosmos::slave']
}
