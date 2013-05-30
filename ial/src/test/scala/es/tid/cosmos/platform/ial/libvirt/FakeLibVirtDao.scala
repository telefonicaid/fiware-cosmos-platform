package es.tid.cosmos.platform.ial.libvirt

import es.tid.cosmos.platform.ial.MachineProfile

/**
 * @author apv
 */
class FakeLibVirtDao extends LibVirtDao {

  val libVirtServers: Seq[LibVirtServerProperties] = Seq(
    new TransientLibVirtServerProperties(
      name = "andromeda02",
      description = "Andromeda 02",
      profile = MachineProfile.M,
      connectionChain = "openvz+ssh://cosmos@192.168.63.12/system?socket=/var/run/libvirt/libvirt-sock",
      numberOfCpus = 4,
      totalMemory = 32768,
      domainTemplate = "centos-6-x86_64",
      bridgeName = "vzbr0",
      domainHostname = "andromeda52",
      domainIpAddress = "192.168.63.62"),
    new TransientLibVirtServerProperties(
      name = "andromeda03",
      description = "Andromeda 03",
      profile = MachineProfile.M,
      connectionChain = "openvz+ssh://cosmos@192.168.63.13/system?socket=/var/run/libvirt/libvirt-sock",
      numberOfCpus = 4,
      totalMemory = 32768,
      domainTemplate = "centos-6-x86_64",
      bridgeName = "vzbr0",
      domainHostname = "andromeda53",
      domainIpAddress = "192.168.63.63"),
    new TransientLibVirtServerProperties(
      name = "andromeda04",
      description = "Andromeda 04",
      profile = MachineProfile.M,
      connectionChain = "openvz+ssh://cosmos@192.168.63.14/system?socket=/var/run/libvirt/libvirt-sock",
      numberOfCpus = 4,
      totalMemory = 32768,
      domainTemplate = "centos-6-x86_64",
      bridgeName = "vzbr0",
      domainHostname = "andromeda54",
      domainIpAddress = "192.168.63.64"),
    new TransientLibVirtServerProperties(
      name = "andromeda05",
      description = "Andromeda 05",
      profile = MachineProfile.M,
      connectionChain = "openvz+ssh://cosmos@192.168.63.15/system?socket=/var/run/libvirt/libvirt-sock",
      numberOfCpus = 4,
      totalMemory = 32768,
      domainTemplate = "centos-6-x86_64",
      bridgeName = "vzbr0",
      domainHostname = "andromeda55",
      domainIpAddress = "192.168.63.65"),
    new TransientLibVirtServerProperties(
      name = "andromeda06",
      description = "Andromeda 06",
      profile = MachineProfile.M,
      connectionChain = "openvz+ssh://cosmos@192.168.63.16/system?socket=/var/run/libvirt/libvirt-sock",
      numberOfCpus = 4,
      totalMemory = 32768,
      domainTemplate = "centos-6-x86_64",
      bridgeName = "vzbr0",
      domainHostname = "andromeda56",
      domainIpAddress = "192.168.63.66")
  )
}
