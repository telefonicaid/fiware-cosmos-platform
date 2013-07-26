class openvz::params {
      $virt_type        = 'openvz'
      $servicename      = 'vz' # vz or kvm service
      $basedir          = '/etc/vz' 
      $confdir          = '/etc/vz/conf'
      $vedir            = '/var/lib/vz'
    
  case $::operatingsystem {
    'RedHat', 'CentOS', 'Fedora': {
       $packages = $virt_type ? {
         kvm => [ 'qemu-kvm', 'libvirt', 'libvirt-daemon-kvm', 'python-virtinst', 'ruby-libvirt', 'libvirt-java' ],
         openvz =>  [ 'vzkernel', 'vzctl', 'vzquota', 'vzstats', 'vzctl-core', 'vzkernel-firmware', 'vzkernel-headers']
       }
    }

    default: {
      fail ("This module is not supported on $operatingsystem")
    }
  }

}
