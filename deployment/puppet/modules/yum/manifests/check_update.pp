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

class yum::check_update {

  package { 'yum':
    ensure      => installed,
  }

  exec { 'yum_check_update':
    command     => '/usr/bin/yum check-update -q || true',
  }

  Package['yum']
    -> Exec['yum_check_update']

}
