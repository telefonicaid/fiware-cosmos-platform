# Agent Class for Cosmos
#
#
# This module will install ambari-server from the Cosmos platform

class cosmos::agent inherits cosmos {

  if ( $cosmos_repo  == yes) {
    include cosmos::common::repos
  }
  
  $package_name  = 'ambari-agent'

  package { "${package_name}" :
    ensure  => 'installed'
    require => Yumrepo('cosmos-ambari'),
  }
}

