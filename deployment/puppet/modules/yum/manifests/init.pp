#
# Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
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
  anchor { 'yum::begin': }
  anchor { 'yum::end': }

  include yum::check_update, yum::params, yum::remove_repos, yum::post_clean,
    yum::thirdparty::puppetlabs, yum::thirdparty::epel

# When the name changes to common:yum, make a grep of yum::thirdparty::epel
# and yum::rhel::optional in initiatives' code, in case anyone if using it
# (and in modules, because develenv uses it)
  case $::operatingsystem {
    'RedHat': {
      $os_repo = 'yum::rhel::base'
    }
    'CentOS': {
      $os_repo = 'yum::centos::base'
      include yum::centos::updates  #It's giving a lot of problems
      Class['yum::post_clean']
        ~> Class['yum::centos::updates']
        -> Anchor['yum::end']

    }
  }

  include $os_repo

  Anchor['yum::begin']
    -> Class['yum::check_update']
    -> Class['yum::remove_repos']
    ~> Class['yum::post_clean']
    -> Class['yum::params']
    -> Class[$os_repo]
    -> Class['yum::thirdparty::puppetlabs', 'yum::thirdparty::epel']
    -> Anchor['yum::end']
}
