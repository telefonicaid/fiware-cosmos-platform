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

# Class: yum
#
# Description:
# This class finds the operating and calls the specific subclass's
# to create the repo.
#
# Repos available but not installed:
# yum::rhel::optional
# yum::thirdparty::epel

class yum {

# When the name changes to common:yum, make a grep of yum::thirdparty::epel
# and yum::rhel::optional in initiatives' code, in case anyone if using it
# (and in modules, because develenv uses it)
  case $::operatingsystem {
    'RedHat': {
      $os_repo = 'yum::rhel::base'
    }
    'CentOS': {
      $os_repo = 'yum::centos::base'
      #include yum::centos::updates  #It's giving a lot of problems
    }
  }

  include yum::remove_repos, yum::post_clean, $os_repo, yum::thirdparty::puppetlabs

  anchor { 'yum::begin': }
    -> Class['yum::remove_repos']
    ~> Class['yum::post_clean']
    -> Class[$os_repo]
    -> Class['yum::thirdparty::puppetlabs']
    -> anchor { 'yum::end': }
}
