# /etc/puppet/manifests/cosmos_deploy.pp

#import 'classes/*'

hosts = [ 'localhost']

node cosmos-ambari-server {
    include ssh
    include mysql
    include libvirt
    include openvz

    user { 'root':
        uid => '0',
        password => '$1$FlfcEIl7$KSvBy03s0V7RtkYKLShaI1',
    }
    file { '/etc/hosts':
        source => 'puppet://cosmos-puppet/files/hosts',
        owner => 'root',
        group => 'root',
        mode  => '0440',
    }
    file { '/etc/sysconfig/network-scripts/route-eth0':
        source => 'puppet://cosmos-puppet/files/route-eth0',
        owner => 'root',
        group => 'root',
        mode  => '0440',
    }
    file { '/etc/ssh/sshd_config':
        source => 'puppet://cosmos-puppet/files/sshd_config',
        owner => 'root',
        group => 'root',
        mode  => '0440',
    }
    service { 'sshd':
        ensure => running,
        require => Package['openssh-server'],
        subscribe => File['/etc/ssh/sshd_config'],
    }
    ssh_authorized_key { 'sshkey_root':
        ensure => present,
        key    => 'AAAAB3NzaC1yc2EAAAADAQABAAABAQC23yBMglb1k80BwUVu7JpUkasEs6oq7Hh6e9/1jJtBN/fModyBLAVFVtzucam4PtpHS3AoQcywlsN4cNZ3jyjGiAMgK+XWIhasmTi0cezTlVp6wclyytiSpNQumy+boGPHKFa89p0DvPrnVSe97Omz5SLHXSLaDYNw+28zzhPoexcw4Qx4Xii2TqfBD7r2ZQRW3of+Mnzcl6+lDpP/FwzJWsKNBurzG/NtHQR5pt19c7YUgwLa7UGzsVJiy8pvA8YwDJLsSJl6QqbAxjzdix/gBjhMfi0DTJSO+CLqBnGbIk36qPfuziDKwrCOYvmGEfHdYf0O6FWFEIhTkC/Gp3J7',
        type   => 'rsa',
        user   => 'root',
    }
    ssh_authorized_key { 'sshkey_sysadmin':
        ensure => present,
        key    => 'AAAAB3NzaC1yc2EAAAADAQABAAABAQC23yBMglb1k80BwUVu7JpUkasEs6oq7Hh6e9/1jJtBN/fModyBLAVFVtzucam4PtpHS3AoQcywlsN4cNZ3jyjGiAMgK+XWIhasmTi0cezTlVp6wclyytiSpNQumy+boGPHKFa89p0DvPrnVSe97Omz5SLHXSLaDYNw+28zzhPoexcw4Qx4Xii2TqfBD7r2ZQRW3of+Mnzcl6+lDpP/FwzJWsKNBurzG/NtHQR5pt19c7YUgwLa7UGzsVJiy8pvA8YwDJLsSJl6QqbAxjzdix/gBjhMfi0DTJSO+CLqBnGbIk36qPfuziDKwrCOYvmGEfHdYf0O6FWFEIhTkC/Gp3J7',
        type   => 'rsa',
        user   => 'sysadmin',
    }
    file { '/etc/yum.repos.d/epg-epel.repo':
        mode => '0660',
        owner => 'root',
        group => 'root',
        source => 'puppet://cosmos-puppet/files/epg-epel.repo',
    }
    package { 'mlocate':
        ensure => 'installed',
    }
}

node 'cosmos-ambari-agent' {
    package { 'python-pip':
        ensure => 'latest',
    }
    file { '/usr/bin/pip':
        ensure => 'link',
        target => '/usr/bin/pip-python',
    }
    package { 'python-setuptools':
        ensure => 'installed',
    }
    package { 'mako':
        ensure => 'installed',
        provider => 'pip',
    }
    package { 'pyOpenSSL':
        ensure => 'installed',
        provider => 'pip',
    }
}

