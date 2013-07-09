class yum::thirdparty::epel($repo_server = 'servilinux.hi.inet') inherits yum::variables {

  yumrepo { 'epel':
    descr    => "Extra Packages for Enterprise Linux $operatingsystemmajrelease",
    enabled  => '1',
    gpgcheck => '0',
    baseurl  => "http://${repo_server}/epel/$operatingsystemmajrelease/\$basearch/",
  }
}
