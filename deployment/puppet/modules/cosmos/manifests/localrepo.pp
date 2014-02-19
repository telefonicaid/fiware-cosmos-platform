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

class cosmos::localrepo inherits cosmos::params {

  package { 'createrepo':
    ensure => present,
  }

  file { 'repodirparent':
    path => $cosmos::params::cosmos_basedir,
    ensure => 'directory',
  }

  file { 'repodir':
    path => "${cosmos::params::cosmos_basedir}/rpms",
    ensure => 'directory',
    recurse => true,
    source => 'puppet:///modules/cosmosplatform',
  }

  exec { 'createrepo':
    command   => "/usr/bin/createrepo ${cosmos::params::cosmos_basedir}/rpms",
    path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
    logoutput => true,
    timeout   => 600,
  }

  yumrepo { 'localrepo':
    descr    => "Comos local repo",
    enabled  => '1',
    gpgcheck => '0',
    baseurl  => "file://${cosmos::params::cosmos_basedir}/rpms",
  }

  Package['createrepo']
    -> File['repodirparent']
    -> File['repodir']
    ~> Exec['createrepo']
    ~> Yumrepo['localrepo']

}
