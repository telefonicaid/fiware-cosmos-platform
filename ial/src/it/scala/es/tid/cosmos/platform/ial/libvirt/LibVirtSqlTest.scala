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

import org.scalatest.Suite

import es.tid.cosmos.platform.common.scalatest.matchers.ContainSomeMatcher
import es.tid.cosmos.platform.ial.MachineProfile

/**
 * A convenient trait for testing LVIP agains MySQL database.
 */
trait LibVirtSqlTest extends MySqlTest {
  this: Suite =>

  override val dbUser = "cosmos"
  override val dbPassword = "cosmos"
  override val dbHost = "localhost"
  override val dbPort = 3306

  val dao = new SqlLibVirtDao(db)

  override val schema = LibVirtDb

  val servers = List(
    new PersistentLibVirtServerProperties(
      name = "andromeda02",
      description = "Andromeda 02",
      profile = MachineProfile.G1Compute,
      connectionChain = "openvz+ssh://cosmos@192.168.63.12/system?socket=/var/run/libvirt/libvirt-sock",
      domainHostname = "andromeda52",
      domainIpAddress = "192.168.63.62",
      enabled = true),
    new PersistentLibVirtServerProperties(
      name = "andromeda03",
      description = "Andromeda 03",
      profile = MachineProfile.G1Compute,
      connectionChain = "openvz+ssh://cosmos@192.168.63.13/system?socket=/var/run/libvirt/libvirt-sock",
      domainHostname = "andromeda53",
      domainIpAddress = "192.168.63.63",
      enabled = true),
    new PersistentLibVirtServerProperties(
      name = "andromeda04",
      description = "Andromeda 04",
      profile = MachineProfile.G1Compute,
      connectionChain = "openvz+ssh://cosmos@192.168.63.14/system?socket=/var/run/libvirt/libvirt-sock",
      domainHostname = "andromeda54",
      domainIpAddress = "192.168.63.64",
      enabled = true),
    new PersistentLibVirtServerProperties(
      name = "andromeda05",
      description = "Andromeda 05",
      profile = MachineProfile.G1Compute,
      connectionChain = "openvz+ssh://cosmos@192.168.63.15/system?socket=/var/run/libvirt/libvirt-sock",
      domainHostname = "andromeda55",
      domainIpAddress = "192.168.63.65",
      enabled = true),
    new PersistentLibVirtServerProperties(
      name = "andromeda06",
      description = "Andromeda 06",
      profile = MachineProfile.G1Compute,
      connectionChain = "openvz+ssh://cosmos@192.168.63.16/system?socket=/var/run/libvirt/libvirt-sock",
      domainHostname = "andromeda56",
      domainIpAddress = "192.168.63.66",
      enabled = true)
  )

  override def populateInitialData {
    LibVirtDb.servers.insert(servers)
  }

  object containServer {
    def apply(name: String) = new ContainSomeMatcher[LibVirtServerProperties](
      s"named $name")(srv => srv.name == name)
  }
}
