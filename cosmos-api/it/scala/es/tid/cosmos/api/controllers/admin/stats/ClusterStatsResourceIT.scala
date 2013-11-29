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

import org.mockito.Mockito.when
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.{JsObject, Json, JsArray}
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.{MaintenanceModeBehaviors, AuthBehaviors}
import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.servicemanager.clusters.ClusterId

class ClusterStatsResourceIT
  extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors{

  val request = FakeRequest(GET, "/cosmos/v1/stats/clusters")

  "Cluster stats" must behave like operatorOnlyResource(request)

  it must "have an empty listing when no clusters are running" in new WithSampleSessions {
    when(services.serviceManager().clusterIds).thenReturn(Seq.empty)
    val response = opUser.doRequest(request)
    status(response) must be (OK)
    (contentAsJson(response) \ "clusters" \\ "id") must have size 0
  }

  it must "list running clusters" in new WithSampleSessions {
    val activeClusterId = services.serviceManager().createCluster(
      name = "active",
      clusterSize = 2,
      serviceDescriptions = Seq.empty,
      users = Seq.empty
    )
    val terminatedClusterId = ClusterId()
    dao.withTransaction { implicit c =>
      dao.assignCluster(activeClusterId, regUser.cosmosProfile.id)
      dao.assignCluster(terminatedClusterId, opUser.cosmosProfile.id)
    }

    val json = contentAsJson(opUser.doRequest(request))
    println(json.toString)

    (json \ "clusters").as[JsArray].value.map(_.as[JsObject]) must contain (Json.obj(
      "id" -> activeClusterId.toString,
      "name" -> "active",
      "ownerHandle" -> regUser.handle,
      "size" -> 2
    ))
  }
}
