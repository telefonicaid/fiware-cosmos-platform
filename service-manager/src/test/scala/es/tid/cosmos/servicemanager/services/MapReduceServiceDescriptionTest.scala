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

package es.tid.cosmos.servicemanager.services

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

/**
 * @author adamos
 */
class MapReduceServiceDescriptionTest extends FlatSpec with MustMatchers {

  "A MapReduceDescription" must "have a jobtracker, tasktracker and mapreduce client" in {
    val description = MapReduceServiceDescription
    description.name must equal ("MAPREDUCE")
    description.components must (
      have length (3) and
      contain (new ComponentDescription("JOBTRACKER", isMaster = true)) and
      contain (new ComponentDescription("TASKTRACKER", isMaster = false)) and
      contain (new ComponentDescription("MAPREDUCE_CLIENT", isMaster = true)))
    val contributions = description.contributions("aMasterNodeName")
    contributions.global must be ('defined)
    contributions.core must be ('empty)
    contributions.services must have length(1)
  }
}
