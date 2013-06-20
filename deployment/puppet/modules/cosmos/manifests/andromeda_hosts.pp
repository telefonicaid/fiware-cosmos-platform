# Manifest to define andromeda hosts
class andromeda_hosts {

  file { '/etc/hosts' :
  ensure => 'present'
  content => template("${module_name}/andromeda_hosts.erb"),
  group   => '0',
  mode    => '644',
  owner   => '0',
  }
}

