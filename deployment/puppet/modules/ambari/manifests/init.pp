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
  include ambari_repos, ambari::install, ambari::config, ambari::service

  Class['ambari_repos'] -> Class['ambari::install'] -> Class['ambari::config'] -> Class['ambari::service']
  Class['ambari::config'] ~> Class['ambari::service']
}
