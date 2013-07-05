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

class cosmos::firewall::firewall_app {
  firewall { '100 allow 8080 access only from localhost':
    dport   => 8080,
    proto  => tcp,
    action => accept,
    source => '127.0.0.1',
  }

  firewall { '101 deny 8080 access from outside':
    dport   => 8080,
    proto  => tcp,
    action => drop,
  }

  Firewall['100 allow 8080 access only from localhost'] -> Firewall['101 deny 8080 access from outside']
}
