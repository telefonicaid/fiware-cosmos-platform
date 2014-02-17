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

class MapReduce2Test extends FlatSpec with MustMatchers {

  "A MapReduce service" must "have a jobtracker, tasktracker and mapreduce client" in {
    val description = MapReduce2
    description.name must equal ("MAPREDUCE2")
    description.components must (
      have length 2 and
        contain (ComponentDescription.masterComponent("HISTORYSERVER")) and
        contain (ComponentDescription.masterComponent("MAPREDUCE2_CLIENT").makeClient))
  }
}
