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
  include ambari::params, ambari_repos, ambari::install, ambari::config, ambari::service

  anchor { 'ambari::begin': }
    -> Class['ambari::params']
    -> Class['ambari_repos']
    -> Class['ambari::install']
    ~> Class['ambari::config']
    ~> Class['ambari::service']
    -> anchor { 'ambari::end': }
}
