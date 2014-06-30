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

define epel::rpm_gpg_key($path) {
  # Given the path to a key, see if it is imported, if not, import it
  exec {  "import-$name":
    path      => '/bin:/usr/bin:/sbin:/usr/sbin',
    command   => "rpm --import $path",
    unless    => "rpm -q gpg-pubkey-$(echo $(gpg --throw-keyids < $path) | cut --characters=11-18 | tr [A-Z] [a-z])",
    require   => File[$path],
    logoutput => 'on_failure',
  }
}
