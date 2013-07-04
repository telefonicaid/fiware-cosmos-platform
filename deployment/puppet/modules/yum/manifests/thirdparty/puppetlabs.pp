class yum::thirdparty::puppetlabs($repo_server = 'servilinux.hi.inet') inherits yum::variables {

  yumrepo { 'puppetlabs-products':
    descr    => "Puppet Labs Products - $operatingsystemmajrelease",
    enabled  => '1',
    gpgcheck => '0',
    baseurl  => "http://${repo_server}/puppet/yum/el/$operatingsystemmajrelease/products/\$basearch/",
  }

  yumrepo { 'puppetlabs-deps':
    descr    => "Puppet Labs Dependencies - $operatingsystemmajrelease",
    enabled  => '1',
    gpgcheck => '0',
    baseurl  => "http://${repo_server}/puppet/yum/el/$operatingsystemmajrelease/dependencies/\$basearch/",
  }
}
