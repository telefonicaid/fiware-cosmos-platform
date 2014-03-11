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

  file { $cosmos_stack_repo_path:
    ensure  => 'directory',
    recurse => true,
    purge   => true,
    force   => true,
    source  => 'puppet:///modules/cosmosplatform',
  }

  exec { 'createrepo':
    command   => "/usr/bin/createrepo ${cosmos_stack_repo_path}",
    path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
    logoutput => true,
    timeout   => 600,
  }

  if $ambari::params::enable_repo_mirroring {

    package{ 'yum-utils':
      ensure => present,
    }

    file { 'reposync-repo' :
      ensure    => present,
      owner     => 'root',
      group     => 'root',
      mode      => '0644',
      path      => '/tmp/reposync.repo',
      content   => template("${module_name}/reposync.repo.erb"),
    }

    exec { 'reposync_ambari':
      command => "reposync -c /tmp/reposync.repo -r ambari-local -p ${cosmos_stack_repo_path}/ambari -n -d",
      path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
      logoutput => true,
      timeout   => 6000,
    }

    exec { 'reposync_hdp':
      command => "reposync -c /tmp/reposync.repo -r HDP-2.0.6-local -p ${cosmos_stack_repo_path}/hdp -n -d",
      path      => [ '/sbin', '/bin', '/usr/sbin', '/usr/bin' ],
      logoutput => true,
      timeout   => 6000,
    }

    File[$cosmos_stack_repo_path]
      -> Exec['reposync_ambari']

    File['reposync-repo']
      -> Exec['reposync_ambari']

    Package['yum-utils']
      -> Exec['reposync_ambari']
      -> Exec['createrepo']

    File[$cosmos_stack_repo_path]
    -> Exec['reposync_hdp']

    File['reposync-repo']
      -> Exec['reposync_hdp']

    Package['yum-utils']
      -> Exec['reposync_hdp']
      -> Exec['createrepo']

  } else {

    yumrepo { 'localrepo':
      descr    => "Comos local repo",
      enabled  => '1',
      gpgcheck => '0',
      baseurl  => "file://${cosmos_stack_repo_path}",
    }

    Exec['createrepo']
      -> Yumrepo['localrepo']

  }

  File[$cosmos::params::cosmos_basedir]
    -> File[$cosmos_stack_repo_path]
    -> Exec['createrepo']

  Package['createrepo']
    -> Exec['createrepo']
}
