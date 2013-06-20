class cosmos::common inherits cosmos::params {

 file {
    [ "${cosmos_basedir}" ;
    "${cosmos_storage}" ;
    "${cosmos_conf_dir}" ;
    "${cosmos_log_dir}" ;
    "${cosmos_init_dir}" ; ] :
    ensure => 'directory',
    mode   => '0755',
    owner  => "${name}",
    group  => "${name}",
  }

}
