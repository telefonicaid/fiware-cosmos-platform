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

class MapReduceTest extends FlatSpec with MustMatchers {
  "A MapReduce service" must "have a jobtracker, tasktracker and mapreduce client" in {
    val description = MapReduce
    description.name must equal ("MAPREDUCE")
    description.components must (
      have length (3) and
      contain (ComponentDescription("JOBTRACKER", isMaster = true)) and
      contain (ComponentDescription("TASKTRACKER", isMaster = false)) and
      contain (ComponentDescription("MAPREDUCE_CLIENT", isMaster = true, isClient = true)))
    val contributions = description.contributions(Map(
      ConfigurationKeys.MasterNode -> "aMasterNodeName",
      ConfigurationKeys.MaxMapTasks -> "10",
      ConfigurationKeys.MaxReduceTasks -> "5"
    ))
    contributions.global must be ('defined)
    contributions.core must be ('empty)
    contributions.services must have length(1)
  }
}
