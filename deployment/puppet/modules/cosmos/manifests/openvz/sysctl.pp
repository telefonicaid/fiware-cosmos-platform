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

class cosmos::openvz::sysctl {
  # On Hardware Node we generally need
  # packet forwarding enabled and proxy arp disabled
  sysctl { 'net.ipv4.ip_forward': val => '1' }
  sysctl { 'net.ipv6.conf.default.forwarding': val => '1' }
  sysctl { 'net.ipv6.conf.all.forwarding': val => '1' }
  sysctl { 'net.ipv4.conf.default.proxy_arp': val => '0' }

  # Enables source route verification
  sysctl { 'net.ipv4.conf.all.rp_filter': val => '1' }

  # Enables the magic-sysrq key
  sysctl { 'kernel.sysrq': val => '1' }

  # We do not want all our interfaces to send redirects
  sysctl { 'net.ipv4.conf.default.send_redirects': val => '1' }
  sysctl { 'net.ipv4.conf.all.send_redirects': val => '0' }
}
