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

class yum::remove_repos {
  package {'epel-release': ensure => absent, }

  file {'/etc/yum.repos.d/epel.repo.rpmsave':     ensure => absent, }
  file {'/etc/yum.repos.d/puppetlabs.repo':       ensure => absent, }
  file {'/etc/yum.repos.d/epg.repo':              ensure => absent, }
  file {'/etc/yum.repos.d/epg-puppet.conf':       ensure => absent, }
  file {'/etc/yum.repos.d/epg-puppet.repo':       ensure => absent, }
  file {'/etc/yum.repos.d/CentOS-Base.repo':      ensure => absent, }
  file {'/etc/yum.repos.d/CentOS-Debuginfo.repo': ensure => absent, }
  file {'/etc/yum.repos.d/CentOS-Media.repo':     ensure => absent, }
  file {'/etc/yum.repos.d/CentOS-Plus.repo':      ensure => absent, }
  file {'/etc/yum.repos.d/CentOS-Contrib.repo':   ensure => absent, }
  file {'/etc/yum.repos.d/CentOS-Extras.repo':    ensure => absent, }
  file {'/etc/yum.repos.d/Centos-Vault.repo':     ensure => absent, }
  file {'/etc/yum.repos.d/Centos-Updates.repo':   ensure => absent, }
  file {'/etc/yum.repos.d/rhel.repo':          	  ensure => absent, }
  file {'/etc/yum.repos.d/redhat.repo':        	  ensure => absent, }
  file {'/etc/yum.repos.d/ambari.repo':           ensure => absent, }
  file {'/etc/yum.repos.d/ambari-remote.repo':    ensure => absent, }
  file {'/etc/yum.repos.d/localrepo.repo':        ensure => absent, }
}
