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

class hostname($hostname, $ip) {

  file { "/etc/hostname":
    ensure => present,
    owner => root,
    group => root,
    mode => 644,
    content => "$hostname\n",
  }

  exec { "set-hostname":
    command => "/bin/hostname -F /etc/hostname",
    unless => "/usr/bin/test `hostname` = `/bin/cat /etc/hostname`",
  }

  host { $hostname:
    ip => $ip,
  }

  File["/etc/hostname"]
    -> Exec["set-hostname"]

}
