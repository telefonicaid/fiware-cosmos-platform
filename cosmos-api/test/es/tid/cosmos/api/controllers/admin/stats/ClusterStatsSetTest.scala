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
import play.api.libs.json.{JsString, Json}

class ClusterStatsSetTest extends FlatSpec with MustMatchers {

  "A cluster stats set" must "be represented in JSON sorted by cluster id" in {
    val stats = ClusterStatsSet(Set(
      ClusterStats(id = "1111", name = "cluster1", ownerHandle = "jsmith", size = 4),
      ClusterStats(id = "0000", name = "cluster0", ownerHandle = "jsmith", size = 10)
    ))
    (Json.toJson(stats) \\ "id") must be (Seq(JsString("0000"), JsString("1111")))
  }
}
