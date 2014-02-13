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
