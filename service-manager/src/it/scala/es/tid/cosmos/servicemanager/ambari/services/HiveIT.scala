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

import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys._
import es.tid.cosmos.servicemanager.services.Hive

class HiveIT extends ConfiguredServiceTest {

  override val dynamicProperties = Map(
    MasterNode -> "aMasterNodeName"
  )

  override def configurator = AmbariHive.configurator(None, resourcesConfigDirectory)

  "The Hive service" must "have global, core and service configuration contributions" in {
    contributions.global must be ('defined)
    contributions.core must be ('defined)
    contributions.services must have length 1
  }
}
