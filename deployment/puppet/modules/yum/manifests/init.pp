# Class: yum
#
# Description:
# This class finds the operating and calls the specific subclass's
# to create the repo.
#
# Repos available but not installed:
# yum::rhel::optional
# yum::thirdparty::epel

class yum {

# Quan canvii el nom a common::yum fer un grep de yum::thirdparty::epel i de yum::rhel::optional a initiatives, per si algu els esta fent servir (i a modules, perque develenv ho fa servir)
  case $::operatingsystem {
    'RedHat': {
      $os_repo = 'yum::rhel::base'
    }
    'CentOS': {
      $os_repo = 'yum::centos::base'
      #include yum::centos::updates  #It's giving a lot of problems
    }
  }

  include yum::remove_repos, yum::post_clean, $os_repo, yum::thirdparty::puppetlabs

  anchor { 'yum::begin': }
    -> Class['yum::remove_repos']
    ~> Class['yum::post_clean']
    -> Class[$os_repo]
    -> Class['yum::thirdparty::puppetlabs']
    -> anchor { 'yum::end': }
}
