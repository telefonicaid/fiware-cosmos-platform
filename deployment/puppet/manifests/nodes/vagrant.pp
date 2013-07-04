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
  include cosmos::master
  Class['yum'] -> Class['cosmos::master']
}

node 'cosmos-slave1' inherits default {
  $ip = "192.168.11.21"
  $ct_ip = "192.168.11.71"
  $ct_hostname = "vagrant-compute01"
  $netmask = "255.255.255.0"
  include cosmos::slave
}

node 'cosmos-slave2' inherits default {
  $ip = "192.168.11.22"
  $ct_ip = "192.168.11.72"
  $ct_hostname = "vagrant-compute02"
  $netmask = "255.255.255.0"
  include cosmos::slave
}
