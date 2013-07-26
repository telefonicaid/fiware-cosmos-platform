class openvz  (
    $vz_utils_repo,   
    $vz_kernel_repo,
    $vz_repo_name,
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
    path      => "$openvz::params::basedir/vz.conf",
    ensure    => present,
    source    => "puppet:///modules/${module_name}/vz.conf",
    notify    => Service[$openvz::params::servicename],
  }
  
  file { "${vz_repo_name}-repo" :
    ensure    => present,
    owner     => 'root',
    group     => 'root',
    mode      => '0644',
    path      => "/etc/yum.repos.d/${vz_repo_name}.repo",
    content   => template("${module_name}/openvz.repo.erb"),
  }

  File['cosmos-openvz-repo']
    -> Package[$openvz::params::packages]
    -> File['vz.conf']
    -> Class['openvz::sysctl']
    -> Service[$openvz::params::servicename]

  anchor {'openvz::begin':}
    -> Class['openvz::sysctl']
    -> anchor {'openvz::end':}
}
