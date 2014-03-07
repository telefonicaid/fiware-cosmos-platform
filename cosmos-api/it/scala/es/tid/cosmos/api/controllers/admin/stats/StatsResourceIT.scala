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

import org.mockito.BDDMockito.given
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.{JsObject, Json, JsArray}
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.{MaintenanceModeBehaviors, AuthBehaviors}
import es.tid.cosmos.api.controllers.pages.WithSampleSessions
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
    val activeClusterId = services.serviceManager().createCluster(
      name = ClusterName("active"),
      clusterSize = 2,
      serviceDescriptions = Seq.empty,
      users = Seq.empty
    )
    val terminatedClusterId = ClusterId()
    dao.withTransaction { implicit c =>
      dao.cluster.register(activeClusterId, regUser.cosmosProfile.id)
      dao.cluster.register(terminatedClusterId, opUser.cosmosProfile.id)
    }

    val json = contentAsJson(opUser.doRequest(clusterStatsRequest))

    val expectedCluster = (json \ "clusters").as[JsArray].value
      .map(_.as[JsObject])
      .find(cluster => {
        (cluster \ "id").as[String] == activeClusterId.toString
      }).get
    (expectedCluster \ "name").as[String] must be ("active")
    (expectedCluster \ "ownerHandle").as[String] must be (regUser.handle)
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
