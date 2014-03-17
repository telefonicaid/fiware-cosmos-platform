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

import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys._

class MapReduce2IT extends ConfiguredServiceTest {

  override val dynamicProperties = Map(
    MasterNode -> "aMasterNodeName",
    MrAppMasterMemory -> "100",
    MapTaskMemory -> "200",
    MapHeapMemory -> "100",
    MaxMapTasks -> "8",
    ReduceTaskMemory -> "100",
    ReduceHeapMemory -> "50",
    MaxReduceTasks -> "4"
  )

  override def configurator = AmbariMapReduce2.configurator((), resourcesConfigDirectory)

  "A MapReduce service" must "have global, core and service configuration contributions" in {
    contributions.global must be ('defined)
    contributions.core must be ('defined)
    contributions.services must have length 1
  }
}
