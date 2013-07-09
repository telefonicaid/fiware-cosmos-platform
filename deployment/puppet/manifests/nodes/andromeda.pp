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

node 'andromeda01' inherits default {
  include cosmos::master, firewall, cosmos::firewall::firewall_pre
  Class['yum'] ~> Class['cosmos::master']

  resources { "firewall":
    purge => true
  }
  Class['cosmos::firewall::firewall_post'] -> Firewall{}
}

node /^andromeda(02|03|04|05|06|07|08|99)$/ inherits default {
  include cosmos::slave
}
