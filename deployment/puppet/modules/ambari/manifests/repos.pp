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

class ambari::repos inherits ambari::params {

  if $ambari::params::enable_repo_mirroring {
    yumrepo { 'ambari':
      baseurl  => $ambari::params::cosmos_stack_repo_url,
      descr    => '[Cosmos] Ambari Repository 1.x',
      enabled  => 1,
      gpgcheck => 0,
    }
    yumrepo { 'ambari-remote':
      baseurl  => $ambari::params::repo_url,
      descr    => '[Cosmos] Ambari Repository 1.x',
      enabled  => 1,
      gpgcheck => 0,
      metadata_expire => 900,
    }
  } else {
    yumrepo { 'ambari':
      baseurl  => $ambari::params::repo_url,
      descr    => '[Cosmos] Ambari Repository 1.x',
      enabled  => 1,
      gpgcheck => 0,
      metadata_expire => 900,
    }
  }

  yumrepo { 'HDP-UTILS-1.1.0.15':
    baseurl  => $ambari::params::hdp_utils_repo_url,
    descr    => '[PDI] Hortonworks Data Platform Utils Version - HDP-UTILS-1.1.0.15',
    enabled  => 1,
    gpgcheck => 1,
    gpgkey   => $ambari::params::hdp_utils_gpg_url,
    priority => 1,
  }
}
