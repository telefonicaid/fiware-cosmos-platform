#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#

class hadoop {
    # You can use any other mirror such as http://apache.rediris.es/hadoop
    $hadoop_mirror  = "http://cosmos10.hi.inet:8080/hadoop"
    $hadoop_version = "1.1.2"
    $hadoop_home    = "/opt/hadoop-${hadoop_version}"

    package { "java-1.6.0-openjdk":
	ensure => "present"
    }

    exec { "download_hadoop":
        command => "wget -O /tmp/hadoop.tar.gz ${hadoop_mirror}/common/hadoop-${hadoop_version}/hadoop-${hadoop_version}.tar.gz",
        path    => $path,
        creates => "/tmp/hadoop.tar.gz",
    }

    exec { "unpack_hadoop":
        command => "tar -zxf /tmp/hadoop.tar.gz -C /opt",
        path    => $path,
        creates => "${hadoop_home}-1.0.3",
        require => Exec["download_hadoop"]
    }

    file { "${hadoop_home}/conf/slaves":
        source => "puppet:///modules/hadoop/slaves",
        mode => 644,
        owner => root,
        group => root,
        require => Exec["unpack_hadoop"]
    }

    file { "${hadoop_home}/conf/masters":
        source => "puppet:///modules/hadoop/masters",
        mode => 644,
        owner => root,
        group => root,
        require => Exec["unpack_hadoop"]
    }

    file { "${hadoop_home}/conf/core-site.xml":
        source => "puppet:///modules/hadoop/core-site.xml",
        mode => 644,
        owner => root,
        group => root,
        require => Exec["unpack_hadoop"]
    }

    file { "${hadoop_home}/conf/mapred-site.xml":
        source => "puppet:///modules/hadoop/mapred-site.xml",
        mode => 644,
        owner => root,
        group => root,
        require => Exec["unpack_hadoop"]
    }

    file { "${hadoop_home}/conf/hdfs-site.xml":
        source => "puppet:///modules/hadoop/hdfs-site.xml",
        mode => 644,
        owner => root,
        group => root,
        require => Exec["unpack_hadoop"]
    }

    file { "${hadoop_home}/conf/hadoop-env.sh":
        source => "puppet:///modules/hadoop/hadoop-env.sh",
        mode => 644,
        owner => root,
        group => root,
        require => Exec["unpack_hadoop"]
    }

    file { "/root/.ssh":
        ensure => "directory",
        mode   => 700
    }

    file { "/root/.ssh/id_rsa":
        source  => "puppet:///modules/hadoop/id_rsa",
        mode    => 600,
        owner   => root,
        group   => root,
        require => File["/root/.ssh"]
    }

    file { "/root/.ssh/id_rsa.pub":
        source => "puppet:///modules/hadoop/id_rsa.pub",
        mode => 644,
        owner => root,
        group => root,
        require => File["/root/.ssh"]
    }

    ssh_authorized_key { "ssh_key":
        ensure => "present",
        key    => "AAAAB3NzaC1yc2EAAAADAQABAAABAQDKibr0qf5y/Iko/UfOq9npr8POkYfw6DYCfX4utzcp0tae9ZENpwu/ugDc3dZU6BMAo+T0diOxg8UQ77XXko/o9fzKA8WUtkmvosrUUEcfS/34XRHD0GiAdMSLt7BiAtlc4lJ8x/3S1lfWLlTe9f3+jY4mZKlLZnExvVWNFrtd0uxQdAj3JciisowYbUZpSId2GWdVuUdH+Y1y2y1JkTgtAnXt1lrCiH8WNOJZVkIhOXJM31OviAXGImSDk2JcycYTio81X/3xKua9yHJQ2AFZt5rh6u25s7VGxp85J5yijV9CV4oQDK51sxC8MIVFZ48YZVf2Ya4Bsfbk/AGtX+97",
        type   => "ssh-rsa",
        user   => "root"
    }
}

