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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.ComponentDescription
import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys._

class MapReduce2Test extends FlatSpec with MustMatchers {

  val dynamicProperties = Map(
    MasterNode -> "aMasterNodeName",
    MrAppMasterMemory -> "100",
    MapTaskMemory -> "200",
    MapHeapMemory -> "100",
    MaxMapTasks -> "8",
    ReduceTaskMemory -> "100",
    ReduceHeapMemory -> "50",
    MaxReduceTasks -> "4"
  )

  "A MapReduce service" must "have a jobtracker, tasktracker and mapreduce client" in {
    val description = MapReduce2
    description.name must equal ("MAPREDUCE2")
    description.components must (
      have length 2 and
      contain (ComponentDescription.masterComponent("HISTORYSERVER")) and
      contain (ComponentDescription.masterComponent("MAPREDUCE2_CLIENT").makeClient))
    val contributions = description.contributions(dynamicProperties)
    contributions.global must be ('defined)
    contributions.core must be ('defined)
    contributions.services must have length 1
  }
}
