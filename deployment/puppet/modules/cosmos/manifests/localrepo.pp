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

  define cleanContainerYum {
    exec {"slave yum clean ${title}" :
      command => "ssh ${title} 'vzctl exec 101 \"yum clean all\"'",
      path    => ['/usr/sbin/', '/bin/', '/usr/bin/'],
      onlyif  => "ssh ${title} 'vzctl status 101' | grep running"
    }
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

    $hosts = hiera('slave_hosts')
    cleanContainerYum { $hosts: }

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

    Exec['createrepo']
      -> CleanContainerYum[$hosts]

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
