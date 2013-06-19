class ambari::ambari_server {
  include ambari::ambari_repo

  package { 'ambari-server':
    ensure => installed,
    require => File['/etc/yum.repos.d/ambari.repo'],
  }
}