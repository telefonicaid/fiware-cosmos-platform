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

class ambari {
  require ambari::params
  anchor { 'ambari::begin': }
  ->
  class {'ambari_repos' :}
  ->
  class {'ambari::install' :}
  ->
  class {'ambari::config' :}
  ->
  class {'ambari::service' :}
  ->
  anchor { 'ambari::end': }

  Class['ambari::config'] ~> Class['ambari::service']
}
