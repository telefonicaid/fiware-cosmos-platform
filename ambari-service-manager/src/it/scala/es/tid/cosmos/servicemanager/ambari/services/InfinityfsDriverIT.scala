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

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys.InfinityMasterNode

class InfinityfsDriverIT extends ConfiguredServiceTest {

  override val dynamicProperties = Map(
    InfinityMasterNode -> "andromeda-infinity"
  )

  override def configurator = AmbariInfinityfsDriver.configurator((), resourcesConfigDirectory)

  val contributedConfig = contributions.core.get

  "An infinity service" must "only have core contributions" in {
    contributions.global must not be 'defined
    contributions.core must be ('defined)
    contributions.services must be ('empty)
  }

  it must "have the default infinity's cluster configured" in {
    contributedConfig.properties("fs.infinity.defaultAuthority") must be ("andromeda-infinity:8020")
  }

  it must "configure the infinity HFS driver" in {
    contributedConfig.properties("fs.infinity.impl").toString must include ("InfinityFileSystem")
  }
}
