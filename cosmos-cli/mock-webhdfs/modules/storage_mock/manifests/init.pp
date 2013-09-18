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

class storage_mock {

  package { "lighttpd":
    ensure  => "present",
    require => Yumrepo['epel']
  }

  service { "lighttpd":
    ensure  => "running",
    enable  => true,
    require => Package["lighttpd"]
  }

  file {["/var/www", "/var/www/lighttpd", "/var/www/lighttpd/cosmos",
         "/var/www/lighttpd/cosmos/v1"]:
    ensure  => "directory",
    require => Package["lighttpd"]
  }

  file { "/var/www/lighttpd/cosmos/v1/storage":
    source  => "puppet:///modules/storage_mock/storage",
    require => File["/var/www/lighttpd/cosmos/v1"]
  }
}

