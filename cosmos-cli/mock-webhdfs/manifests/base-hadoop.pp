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

include epel
include hadoop
include storage_mock

yumrepo { "epg":
  baseurl  => "http://repos.hi.inet/redhat/rhel6.4s-x86_64/RPMS.all/",
  cost     => 0,
  descr    => "EPG internal mirror for RedHat 6.4",
  enabled  => 1,
  gpgcheck => 0
}

group { "puppet":
  ensure => "present",
}

package { "vim-minimal":
  ensure => "present",
}

exec { "purge firewall":
  command => "/sbin/iptables -F && /sbin/iptables-save > /etc/sysconfig/iptables && /sbin/service iptables restart",
  onlyif  => "/usr/bin/test `/bin/grep \"Firewall configuration written by\" /etc/sysconfig/iptables | /usr/bin/wc -l` -gt 0",
  user    => 'root',
}
