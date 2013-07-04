# Class: yum::centos:updates
#
# Description:
# This class finds the operating release and calls the yumrepo class
# to create the repo.
#
class yum::centos::updates ($repo_server = 'repos.hi.inet') inherits yum::variables {

  yumrepo { 'Centos-Updates':
    descr      => 'Centos Updates',
    enabled    => '1',
    gpgcheck   => '0',
    baseurl    => "http://${repo_server}/redhat/centos${operatingsystemmajrelease}-\$basearch/RPMS.updates",
  }
}
