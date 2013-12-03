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

package es.tid.cosmos.api.controllers.admin.stats

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import play.api.libs.json.Json

import es.tid.cosmos.platform.ial.MachineProfile._

class MachineStatsTest extends FlatSpec with MustMatchers {

  "MachineStats" must "be converted to JSON" in {
    val stats = MachineStats(
      G1Compute -> ResourceUse(total = 10, available = 1),
      HdfsSlave -> ResourceUse(total = 4, available = 0),
      HdfsMaster -> ResourceUse(total = 1, available = 0)
    )

    Json.toJson(stats) must equal (Json.obj(
      "g1-compute" -> Json.obj("total" -> 10, "available" -> 1),
      "hdfs-slave" -> Json.obj("total" -> 4, "available" -> 0),
      "hdfs-master" -> Json.obj("total" -> 1, "available" -> 0)
    ))
  }

  it must "require stats for all existing profiles" in {
    val ex = evaluating {
      MachineStats(Map(
        G1Compute -> ResourceUse(total = 10, available = 1),
        HdfsSlave -> ResourceUse(total = 4, available = 0)
      ))
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("Missing stats for profile 'hdfs-master'")
  }
}
