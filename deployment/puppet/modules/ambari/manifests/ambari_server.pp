class ambari::ambari_server {
  include ambari::ambari_repos

  package { 'ambari-server':
    ensure => installed,
    require => Yumrepo['ambari-1.x'],
  }
}
