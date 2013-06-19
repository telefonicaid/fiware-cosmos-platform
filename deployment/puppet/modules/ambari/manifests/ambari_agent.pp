class ambari::ambari_agent {
  include ambari::ambari_repo

  package { 'ambari-agent':
    ensure => installed,
    require => File['/etc/yum.repos.d/ambari.repo'],
  }  
}