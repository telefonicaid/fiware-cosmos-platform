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
import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys

class MapReduce2Test extends FlatSpec with MustMatchers {
  "A MapReduce service" must "have a jobtracker, tasktracker and mapreduce client" in {
    val description = MapReduce2
    description.name must equal ("MAPREDUCE2")
    description.components must (
      have length 2 and
      contain (ComponentDescription("HISTORYSERVER", isMaster = true)) and
      contain (ComponentDescription("MAPREDUCE2_CLIENT", isMaster = true, isClient = true)))
    val contributions = description.contributions(Map(
      ConfigurationKeys.MappersPerSlave -> "3",
      ConfigurationKeys.MasterNode -> "aMasterNodeName",
      ConfigurationKeys.MaxMapTasks -> "10",
      ConfigurationKeys.MaxReduceTasks -> "5",
      ConfigurationKeys.ReducersPerSlave -> "1"
    ))
    contributions.global must be ('defined)
    contributions.core must be ('defined)
    contributions.services must have length 1
  }
}
