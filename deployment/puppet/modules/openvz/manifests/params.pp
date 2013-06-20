class openvz::params {

  case $::virtual {
    /^openvzhn/: {
      $servicename = 'vz'
      $basedir = '/etc/vz/'
      $confdir = '/etc/vz/conf/'
      $vedir = '/var/lib/vz/' #XXX: Update here with your custom value
    }
    default: {
      $servicename = 'libvirtd'
    }
  }

  case $::operatingsystem {
    'RedHat', 'CentOS', 'Fedora': {
     # FIXME: not tested
       $packages = $virtual ? {
         kvm => [ 'qemu-kvm', 'libvirt', 'libvirt-daemon-kvm', 'python-virtinst', 'ruby-libvirt', 'libvirt-java' ],
         openvz =>  [ 'vzkernel', 'vzkernel-headers', 'vzctl', 'vzquota' ],
       }
  }

  default: {
    fail ("This module is not supported on $operatingsystem")
    }
  }
}
