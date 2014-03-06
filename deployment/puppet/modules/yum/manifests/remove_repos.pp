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
  file {'/etc/yum.repos.d/localrepo.repo':        ensure => absent, }
}
