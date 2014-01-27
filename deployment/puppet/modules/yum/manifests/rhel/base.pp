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

# Class: yum::rhel
#
# Description:
# This class finds the operating system release and calls the yumrepo class
# to create the repo.
# 
class yum::rhel::base($repo_server = 'repos.hi.inet') {

  yumrepo { 'rhel-base':
    descr    => "Red Hat Enterprise Linux $::operatingsystemrelease - Base",
    enabled  => '1',
    gpgcheck => '0',
    baseurl  => "http://${repo_server}/redhat/rhel${::operatingsystemmajrelease}s-\$basearch/RPMS.all/",
  }

}
