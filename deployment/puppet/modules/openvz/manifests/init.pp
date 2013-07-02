class openvz  (
    $vz_utils_repo,   
    # "http://192.168.30.1/cosmos-deps/ovz/openvz-utils/", "http://cosmos10.hi.inet/develenv/rpms/cosmos-deps/ovz/openvz-utils/"
    $vz_kernel_repo,  
    # "http://192.168.30.1/cosmos-deps/ovz/openvz-kernel-rhel6/",
    #$vz_repo_key,
    
) inherits openvz::params {
  include openvz::sysctl

  package { $openvz::params::packages : 
    ensure    => installed,
  }

  service { $openvz::params::servicename :
    ensure    => running,
    enable    => true,
  }

  file { 'vz.conf' :
    path      => "$openvz::params::confdir/vz.conf",
    ensure    => present,
    source    => "puppet:///modules/openvz/vz.conf",
    notify    => Service[$openvz::params::servicename],
  }
  
  file { 'cosmos-openvz-repo' : 
    ensure    => present,
    owner     => 'root',
    group     => 'root',
    mode      => '0644',
    path      => "/etc/yum.repos.d/cosmos-openvz.repo",
    content   => template('openvz/cosmos-openvz.repo.erb'),
  }
  
  #file { [ $openvz::params::basedir, $openvz::params::confdir, $openvz::params::vedir ] : 
	#ensure    => directory,
    #owner     => 'root',
    #group     => 'root',
    #mode      => '0644',
  #}

  File['cosmos-openvz-repo'] -> Package[$openvz::params::packages] -> File['vz.conf'] -> Class['openvz::sysctl'] -> Service[$openvz::params::servicename]

}
