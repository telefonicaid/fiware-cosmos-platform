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

class pdi_base {
  include pdi_base::params

  # PDI Yum Repositories

  yumrepo { 'CentOS-Base':
    name     => 'CentOS-Base-6.4',
    baseurl  => "http://repos.hi.inet/redhat/centos6.4-${pdi_base::params::basearch}/RPMS.all/",
    descr    => "PDI CentOS 6.4 Base Repository",
    enabled  => 1,
    gpgcheck => 0,
  }

  yumrepo { 'CentOS-Updates':
    name     => 'CentOS-Updates',
    baseurl  => "http://repos.hi.inet/redhat/centos6-${pdi_base::params::basearch}/RPMS.all/",
    descr    => "PDI CentOS Updates Repository",
    enabled  => 1,
    gpgcheck => 0,
  }

  yumrepo { 'epel-pdi':
    name     => 'EPEL-Repo-PDI',
    baseurl  => "http://repos.hi.inet/centos/epel6-${pdi_base::params::basearch}/RPMS.all/",
    descr    => "PDI EPEL Repository",
    enabled  => 1,
    gpgcheck => 0,
  }
}
