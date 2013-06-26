class cosmos::cluster_hosts($environment) {
  file { '/etc/hosts' :
    ensure => 'present',
    content => template("${module_name}/${environment}_hosts.erb"),
    group   => '0',
    mode    => '644',
    owner   => '0',
  }
}
