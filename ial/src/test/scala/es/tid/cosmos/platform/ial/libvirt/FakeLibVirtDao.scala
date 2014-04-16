/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.platform.ial.libvirt

import es.tid.cosmos.platform.ial.MachineProfile

class FakeLibVirtDao extends LibVirtDao {

  private val computeServers =
    for (i <- 1 to 5) yield propertiesForNumber(i, MachineProfile.G1Compute)

  private val hdfsMasterServer = propertiesForNumber(6, MachineProfile.HdfsMaster)

  private val hdfsSlaveServers =
    for (i <- 7 to 9) yield propertiesForNumber(i, MachineProfile.HdfsSlave)

  val libVirtServers = (computeServers :+ hdfsMasterServer) ++ hdfsSlaveServers

  private def propertiesForNumber(
      i: Int, profile: MachineProfile.Value) = new TransientLibVirtServerProperties(
    name = s"andromeda0$i",
    description = s"Andromeda 0$i",
    profile = profile,
    rack = if (i % 2 == 1) "rack01" else "rack02",
    connectionChain = connChain(s"192.168.63.$i"),
    domainHostname = s"andromeda${50 + i}",
    domainIpAddress = s"192.168.63.${50 + i}")

  private def connChain(hostname: String) =
    s"openvz+ssh://cosmos@$hostname/system?socket=/var/run/libvirt/libvirt-sock"
}
