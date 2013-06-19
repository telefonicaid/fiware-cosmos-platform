class ambari::ambari_repo {
  file { '/etc/yum.repos.d/ambari.repo':
    mode => '0660',
    owner => 'root',
    group => 'root',
    source => 'puppet:///modules/ambari/ambari.repo',
  }
}