# Class: yum::rhel
#
# Description:
# This class finds the operating system release and calls the yumrepo class
# to create the repo.
# 
class yum::rhel::optional($repo_server = 'repos.hi.inet') {

  yumrepo { 'rhel-optional':
    descr    => "Red Hat Enterprise Linux $operatingsystemmajrelease - Optional",
    enabled  => '1',
    gpgcheck => '0',
    baseurl  => "http://${repo_server}/redhat/rhel${operatingsystemmajrelease}s-\$basearch/RPMS.optional/",
  }

}
