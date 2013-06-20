class openvz {

  include openvz::params
  package { $openvz::params::packages: ensure => latest }
  
  File {
    owner => 'root',
    group => 'root',
    mode => 0644,
    subscribe => Package[$openvz::params::packages],
  }

  service { $openvz::params::service:
    ensure => running,
    enable => true,
  }

  case $virtual {
    /^openvz/: {
      file {
        "${virt::params::basedir}/vz.conf":
	ensure => present,
	source => template[openvz/vz.conf.erb',
	notify => Service[$openvz::params::servicename];
	[ $openvz::params::confdir, $openvz::params::vedir ]: ensure => directory;
      }
    }
  }
}
