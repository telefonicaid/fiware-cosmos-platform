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

package es.tid.cosmos.api.controllers.info

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json

import es.tid.cosmos.servicemanager.clusters.ClusterId

class InfoTest extends FlatSpec with MustMatchers {

  "Group info" must "be serializable to JSON" in {
    Json.toJson(GroupInfo(name = "groupA", guaranteedQuota = Some(10))) must
      be (Json.obj("name" -> "groupA", "guaranteedQuota" -> 10))
  }

  it must "lack guaranteedQuota field when there is no group quota" in {
    Json.toJson(GroupInfo(name = "groupA", guaranteedQuota = None)) must
      be (Json.obj("name" -> "groupA"))
  }

  "An info value" must "be serializable to JSON" in  {
    val info = Info(
      profileId = 1L,
      handle = "jsmith",
      individualQuota = Some(6),
      group = GroupInfo(name = "cosmos", guaranteedQuota = Some(10)),
      clusters = ClustersInfo(
        owned = Seq(ClusterId("cluster1")),
        accessible = Seq.empty
      ),
      resources = ResourcesInfo(
        groupConsumption = 3,
        individualConsumption = 2,
        available = 100,
        availableForGroup = 7,
        availableForUser = 4
      )
    )
    val json = Json.obj(
      "profileId" -> 1L,
      "handle" -> "jsmith",
      "individualQuota" -> 6,
      "group" -> Json.obj(
        "name" -> "cosmos",
        "guaranteedQuota" -> 10
      ),
      "clusters" -> Json.obj(
        "owned" -> Json.arr("cluster1"),
        "accessible" -> Json.arr()
      ),
      "resources" -> Json.obj(
        "groupConsumption" -> 3,
        "individualConsumption" -> 2,
        "available" -> 100,
        "availableForGroup" -> 7,
        "availableForUser" -> 4
      )
    )
    Json.toJson(info) must be (json)
    Json.toJson(info.copy(individualQuota = None)) must be (json - "individualQuota")
  }
}
