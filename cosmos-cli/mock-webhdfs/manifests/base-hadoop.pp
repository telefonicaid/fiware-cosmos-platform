#
# Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
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

file { "/etc/puppet":
  ensure => "directory"
}

file { "/etc/puppet/puppet.conf":
  source  => "puppet:///modules/vagrant/puppet.conf",
  require => File["/etc/puppet"]
}

package { "vim-enhanced":
  ensure => "present",
}

exec { "purge firewall":
  command => "/sbin/iptables -F && /sbin/iptables-save > /etc/sysconfig/iptables && /sbin/service iptables restart",
  onlyif  => "/usr/bin/test `/bin/grep \"Firewall configuration written by\" /etc/sysconfig/iptables | /usr/bin/wc -l` -gt 0",
  user    => 'root',
}
