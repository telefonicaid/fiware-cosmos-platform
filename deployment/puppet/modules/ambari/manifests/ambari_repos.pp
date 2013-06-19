class ambari::ambari_repos {
  include ambari::params

  # Cosmos and PDI Ambari repositories
  yumrepo { 'ambari-1.x':
    baseurl  => $ambari_repo_url, #parameterize
    descr    => "[Cosmos] Ambari Repository 1.x",
    enabled  => 1,
    gpgcheck => 0,
#    priority => 10,
  }

  yumrepo { 'HDP-UTILS-1.1.0.15':
    baseurl  => 'http://servilinux.hi.inet/ambari/HDP-UTILS-1.1.0.15/',
    descr    => '[PDI] Hortonworks Data Platform Utils Version - HDP-UTILS-1.1.0.15',
    enabled  => 1,
    gpgcheck => 1,
    gpgkey   => 'http://servilinux.hi.inet/ambari/RPM-GPG-KEY-Jenkins',
    priority => 1,
  }

  yumrepo { 'Updates-ambari-1.x':
    baseurl  => 'http://servilinux.hi.inet/ambari/Updates-ambari-1.x/',
    descr     => '[PDI] ambari-1.x - Updates',
    enabled  => 1,
    gpgcheck => 1,
    gpgkey   => 'http://servilinux.hi.inet/ambari/RPM-GPG-KEY-Jenkins',
    priority => 1,
  }

  yumrepo { 'Updates-ambari-1.2.2.4':
    baseurl  => 'http://servilinux.hi.inet/ambari/Updates-ambari-1.2.2.4/',
    descr     => '[PDI] ambari-1.2.2.4 - Updates',
    enabled  => 1,
    gpgcheck => 1,
    gpgkey   => 'http://servilinux.hi.inet/ambari/RPM-GPG-KEY-Jenkins',
    priority => 1,
  }
}
