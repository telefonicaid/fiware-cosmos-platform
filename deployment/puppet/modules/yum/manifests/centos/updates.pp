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

# Class: yum::centos:updates
#
# Description:
# This class finds the operating release and calls the yumrepo class
# to create the repo.
#
class yum::centos::updates ($repo_server = 'repos.hi.inet') {

  yumrepo { 'Centos-Updates':
    descr      => 'Centos Updates',
    enabled    => '1',
    gpgcheck   => '0',
    baseurl    => "http://${repo_server}/redhat/centos${::operatingsystemmajrelease}-\$basearch/RPMS.updates",
  }
}
