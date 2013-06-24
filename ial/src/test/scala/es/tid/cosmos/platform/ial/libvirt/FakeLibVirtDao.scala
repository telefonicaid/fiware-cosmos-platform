/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.platform.ial.libvirt

import es.tid.cosmos.platform.ial.MachineProfile

class FakeLibVirtDao extends LibVirtDao {

  val libVirtServers = for (i <- 1 to 5) yield {
    new TransientLibVirtServerProperties(
      name = s"andromeda0$i",
      description = s"Andromeda 0$i",
      profile = MachineProfile.G1_COMPUTE,
      connectionChain = connChain(s"192.168.63.$i"),
      domainHostname = s"andromeda${50 + i}",
      domainIpAddress = s"192.168.63.${50 + i}"
    )
  }

  private def connChain(hostname: String) =
    s"openvz+ssh://cosmos@$hostname/system?socket=/var/run/libvirt/libvirt-sock"
}
