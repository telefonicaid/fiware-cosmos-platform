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
import play.api.libs.json.{JsObject, Json, JsArray}
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.{MaintenanceModeBehaviors, AuthBehaviors}
import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.profile.ClusterSecret
import es.tid.cosmos.platform.ial.MachineProfile._
import es.tid.cosmos.servicemanager.ClusterName
import es.tid.cosmos.servicemanager.clusters.ClusterId

class StatsResourceIT
  extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors{

  val clusterStatsRequest = FakeRequest(GET, "/cosmos/v1/stats/clusters")

  "Cluster stats" must behave like operatorOnlyResource(clusterStatsRequest)

  it must "have an empty listing when when there are no clusters" in new WithSampleSessions {
    val response = opUser.doRequest(clusterStatsRequest)
    status(response) must be (OK)
    (contentAsJson(response) \ "clusters" \\ "id") must have size 0
  }

  it must "list running clusters" in new WithSampleSessions {
    val (activeClusterId, _) = services.serviceManager.createCluster(
      name = ClusterName("active"),
      clusterSize = 2,
      services = Set.empty,
      users = Seq.empty
    )
    val terminatedClusterId = ClusterId.random()
    store.withTransaction { implicit c =>
      store.cluster.register(activeClusterId, regUserInGroup.cosmosProfile.id, ClusterSecret.random(), shared = false)
      store.cluster.register(terminatedClusterId, opUser.cosmosProfile.id, ClusterSecret.random(), shared = false)
    }

    val json = contentAsJson(opUser.doRequest(clusterStatsRequest))

    val expectedCluster = (json \ "clusters").as[JsArray].value
      .map(_.as[JsObject])
      .find(cluster => {
        (cluster \ "id").as[String] == activeClusterId.toString
      }).get
    (expectedCluster \ "name").as[String] must be ("active")
    (expectedCluster \ "ownerHandle").as[String] must be (regUserInGroup.handle)
    (expectedCluster \ "size").as[Int] must be (2)
    (expectedCluster \ "slaves").as[Seq[String]] must ((have size 2) or (have size 0))
  }

  val machineStatsRequest = FakeRequest(GET, "/cosmos/v1/stats/machines")

  "Machine stats" must behave like operatorOnlyResource(machineStatsRequest)

  it must "list available and total machines" in new WithSampleSessions {
    val response = opUser.doRequest(machineStatsRequest)
    status(response) must be (OK)
    contentAsJson(response) must be (Json.toJson(MachineStats(
      G1Compute -> ResourceUse(total = 100, available = 100),
      HdfsSlave -> ResourceUse(total = 4, available = 4),
      HdfsMaster -> ResourceUse(total = 1, available = 1)
    )))
  }
}
