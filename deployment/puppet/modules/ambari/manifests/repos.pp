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

class ambari::repos inherits ambari::params {

  if $ambari::params::enable_repo_mirroring {
    yumrepo { 'ambari':
      baseurl  => $ambari::params::cosmos_stack_repo_url,
      descr    => '[Cosmos] Ambari Repository 1.x',
      enabled  => 1,
      gpgcheck => 0,
    }
    yumrepo { 'ambari-remote':
      baseurl  => $ambari::params::repo_url,
      descr    => '[Cosmos] Ambari Repository 1.x',
      enabled  => 1,
      gpgcheck => 0,
      metadata_expire => 900,
      proxy    => $cosmos::params::proxy,
    }
  } else {
    yumrepo { 'ambari':
      baseurl  => $ambari::params::repo_url,
      descr    => '[Cosmos] Ambari Repository 1.x',
      enabled  => 1,
      gpgcheck => 0,
      metadata_expire => 900,
      proxy    => $cosmos::params::proxy
    }
  }

  yumrepo { 'HDP-UTILS-1.1.0.15':
    baseurl  => $ambari::params::hdp_utils_repo_url,
    descr    => '[PDI] Hortonworks Data Platform Utils Version - HDP-UTILS-1.1.0.15',
    enabled  => 1,
    gpgcheck => 0,
    priority => 1,
    proxy    => $cosmos::params::proxy
  }
}
