class cosmos::cluster_hosts {
  file { '/etc/hosts' :
    ensure => 'present',
    source => "puppet:///modules/${module_name}/environments/${environment}/hosts",
    group   => '0',
    mode    => '644',
    owner   => '0',
  }
}
