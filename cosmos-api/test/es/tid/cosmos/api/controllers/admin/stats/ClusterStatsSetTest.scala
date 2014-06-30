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
import play.api.libs.json.{JsString, Json}

class ClusterStatsSetTest extends FlatSpec with MustMatchers {

  "A cluster stats set" must "be represented in JSON sorted by cluster id" in {
    val stats = ClusterStatsSet(Set(
      ClusterStats(id = "1111", name = "cluster1", ownerHandle = "jsmith",
        size = 4, master = "master.hi.inet", slaves = Seq()),
      ClusterStats(id = "0000", name = "cluster0", ownerHandle = "jsmith",
        size = 10, master= "master2.hi.inet", slaves = Seq("slave.hi.inet"))
    ))
    (Json.toJson(stats) \\ "id") must be (Seq(JsString("0000"), JsString("1111")))
  }
}
