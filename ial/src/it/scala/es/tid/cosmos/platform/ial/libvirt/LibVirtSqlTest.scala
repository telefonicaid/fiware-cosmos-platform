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

import es.tid.cosmos.platform.common.db.MySqlTest
import es.tid.cosmos.platform.common.scalatest.matchers.ContainSomeMatcher
import es.tid.cosmos.platform.ial.MachineProfile

/**
 * A convenient trait for testing LVIP agains MySQL database.
 */
trait LibVirtSqlTest extends MySqlTest {
  this: Suite =>

  val dbUser = "cosmos"
  val dbPassword = "cosmos"
  val dbHost = "localhost"
  val dbPort = 3306

  val dao = new SqlLibVirtDao(db)

  val schema = LibVirtDb

  val servers = List(
    new PersistentLibVirtServerProperties(
      name = "andromeda02",
      description = "Andromeda 02",
      MachineProfile.M,
      connectionChain = "openvz+ssh://cosmos@192.168.63.12/system?socket=/var/run/libvirt/libvirt-sock",
      numberOfCpus = 4,
      totalMemory = 32768,
      domainTemplate = "centos-6-x86_64",
      bridgeName = "vzbr0",
      domainHostname = "andromeda52",
      domainIpAddress = "192.168.63.62",
      true),
    new PersistentLibVirtServerProperties(
      name = "andromeda03",
      description = "Andromeda 03",
      MachineProfile.M,
      connectionChain = "openvz+ssh://cosmos@192.168.63.13/system?socket=/var/run/libvirt/libvirt-sock",
      numberOfCpus = 4,
      totalMemory = 32768,
      domainTemplate = "centos-6-x86_64",
      bridgeName = "vzbr0",
      domainHostname = "andromeda53",
      domainIpAddress = "192.168.63.63",
      true),
    new PersistentLibVirtServerProperties(
      name = "andromeda04",
      description = "Andromeda 04",
      MachineProfile.M,
      connectionChain = "openvz+ssh://cosmos@192.168.63.14/system?socket=/var/run/libvirt/libvirt-sock",
      numberOfCpus = 4,
      totalMemory = 32768,
      domainTemplate = "centos-6-x86_64",
      bridgeName = "vzbr0",
      domainHostname = "andromeda54",
      domainIpAddress = "192.168.63.64",
      true),
    new PersistentLibVirtServerProperties(
      name = "andromeda05",
      description = "Andromeda 05",
      MachineProfile.M,
      connectionChain = "openvz+ssh://cosmos@192.168.63.15/system?socket=/var/run/libvirt/libvirt-sock",
      numberOfCpus = 4,
      totalMemory = 32768,
      domainTemplate = "centos-6-x86_64",
      bridgeName = "vzbr0",
      domainHostname = "andromeda55",
      domainIpAddress = "192.168.63.65",
      true),
    new PersistentLibVirtServerProperties(
      name = "andromeda06",
      description = "Andromeda 06",
      MachineProfile.M,
      connectionChain = "openvz+ssh://cosmos@192.168.63.16/system?socket=/var/run/libvirt/libvirt-sock",
      numberOfCpus = 4,
      totalMemory = 32768,
      domainTemplate = "centos-6-x86_64",
      bridgeName = "vzbr0",
      domainHostname = "andromeda56",
      domainIpAddress = "192.168.63.66",
      true)
  )

  override def populateInitialData {
    LibVirtDb.servers.insert(servers)
  }

  object containServer {
    def apply(name: String) = new ContainSomeMatcher[LibVirtServerProperties](
      s"named $name")(srv => srv.name == name)
  }
}
