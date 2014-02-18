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

class ambari::server {
  include ambari::params, ambari::repos, ambari::db, ambari::server::install,
    ambari::server::config, ambari::server::service

  anchor { 'ambari::server::begin': }
    -> Class['ambari::params']
    -> Class['ambari::repos']
    -> Class['ambari::db']
    -> Class['ambari::server::install']
    ~> Class['ambari::server::config']
    ~> Class['ambari::server::service']
    -> anchor { 'ambari::server::end': }
}
