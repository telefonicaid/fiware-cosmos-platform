# Configuration Class for Cosmos API
#
#
# This module will configure cosmos-api for IAL on the Cosmos platform
#
class cosmos::config::api {

  include cosmos::params

  $service_name   = 'cosmos-api'
  
  service { $service_name :
    ensure  => $svc_ensure,
    enable  => $svc_enable,
  }
  
  file { "${service_name}" :
    ensure  => 'link',
    path    => "/etc/init.d/${service_name}",
    target  => "${cosmos_init_dir}/${service_name}",
    mode    => '0755',
    owner   => '0',
    group   => '0',
  }
  
  file { 'cosmos-api.conf' :
    path    => "${cosmos_conf_dir}/${service_name}.conf",
    ensure  => 'present',
    mode    => '0644',
    owner   => "${module_name}",
    group   => "${module_name}",
    content => template("cosmos/${service_name}.conf.erb"),
    notify  => Service("${service_name}"),
  }
  
  file { 'logback.conf' :
    path    => "${cosmos_conf_dir}/logback.conf",
    ensure  => 'present',
    mode    => '0644',
    owner   => "${module_name}",
    group   => "${module_name}",
    content => template('cosmos/logback.conf.erb'),
    notify  => Service("${service_name}"),
  }
}
