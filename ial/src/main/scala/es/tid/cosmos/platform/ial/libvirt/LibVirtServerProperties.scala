package es.tid.cosmos.platform.ial.libvirt

import es.tid.cosmos.platform.ial.MachineProfile

/**
 * The properties for a libvirt server.
 */
trait LibVirtServerProperties {
  val name: String
  val description: String
  val profile: MachineProfile.Value
  val connectionChain: String
  val numberOfCpus: Int
  val totalMemory: Int
  val domainTemplate: String
  val bridgeName: String
  val domainHostname: String
  val domainIpAddress: String
}
