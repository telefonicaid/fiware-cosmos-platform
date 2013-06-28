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

class ambari::ambari_repos {
  include ambari::params

  # Cosmos and PDI Ambari repositories
  yumrepo { 'ambari':
    baseurl  => $ambari::params::ambari_repo_url,
    descr    => "[Cosmos] Ambari Repository 1.x",
    enabled  => 1,
    gpgcheck => 0,
  }

  yumrepo { 'HDP-UTILS-1.1.0.15':
    baseurl  => 'http://servilinux.hi.inet/ambari/HDP-UTILS-1.1.0.15/',
    descr    => '[PDI] Hortonworks Data Platform Utils Version - HDP-UTILS-1.1.0.15',
    enabled  => 1,
    gpgcheck => 1,
    gpgkey   => 'http://servilinux.hi.inet/ambari/RPM-GPG-KEY-Jenkins',
    priority => 1,
  }

  yumrepo { 'Updates-ambari-1.x':
    baseurl  => 'http://servilinux.hi.inet/ambari/Updates-ambari-1.x/',
    descr     => '[PDI] ambari-1.x - Updates',
    enabled  => 0,
    gpgcheck => 1,
    gpgkey   => 'http://servilinux.hi.inet/ambari/RPM-GPG-KEY-Jenkins',
    priority => 1,
  }

  yumrepo { 'Updates-ambari-1.2.2.4':
    baseurl  => 'http://servilinux.hi.inet/ambari/Updates-ambari-1.2.2.4/',
    descr     => '[PDI] ambari-1.2.2.4 - Updates',
    enabled  => 0,
    gpgcheck => 1,
    gpgkey   => 'http://servilinux.hi.inet/ambari/RPM-GPG-KEY-Jenkins',
    priority => 1,
  }
}
