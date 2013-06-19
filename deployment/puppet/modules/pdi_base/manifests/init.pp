class pdi_base {
  # PDI Yum Repositories
  yumrepo { 'CentOS-Base':
    name     => 'CentOS-Base-6.4',
    baseurl  => 'http://repos.hi.inet/redhat/centos6.4-$basearch/RPMS.all/',
    descr    => "PDI CentOS 6.4 Base Repository",
    enabled  => 1,
    gpgcheck => 0,
#    priority => 10,
  }

  yumrepo { 'CentOS-Updates':
    name     => 'CentOS-Updates',
    baseurl  => 'http://repos.hi.inet/redhat/centos6-$basearch/RPMS.all/',
    descr    => "PDI CentOS Updates Repository",
    enabled  => 1,
    gpgcheck => 0,
#    priority => 10,
  }

  yumrepo { 'epel-pdi':
    name     => 'EPEL-Repo-PDI',
    baseurl  => 'http://repos.hi.inet/centos/epel6-x86_64/RPMS.all/',
    descr    => "PDI EPEL Repository",
    enabled  => 1,
    gpgcheck => 0,
#    priority => 10,
  }
}
