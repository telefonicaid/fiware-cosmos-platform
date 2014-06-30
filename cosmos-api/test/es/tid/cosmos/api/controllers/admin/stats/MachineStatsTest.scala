/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
