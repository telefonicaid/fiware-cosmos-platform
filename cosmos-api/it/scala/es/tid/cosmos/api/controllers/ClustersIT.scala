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

package es.tid.cosmos.api.controllers

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.cluster.CreateClusterParams
import es.tid.cosmos.api.mocks.WithSampleUsers
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.servicemanager.ambari.services.Hdfs
import es.tid.cosmos.servicemanager.clusters.ClusterId

class ClustersIT
  extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors {

  val validCreationParams = Json.toJson(CreateClusterParams("cluster_new", 10, Seq(Hdfs.name)))
  val inValidCreationParams = Json.obj("invalid" -> "json")
  val resourcePath = "/cosmos/v1/cluster"
  val listClusters = FakeRequest(GET, resourcePath)
  val createCluster = FakeRequest(POST, resourcePath).withJsonBody(validCreationParams)

  "Cluster listing" must behave like rejectingUnauthenticatedRequests(listClusters)

  it must behave like resourceDisabledWhenUnderMaintenance(listClusters)

  "Cluster creation" must behave like rejectingUnauthenticatedRequests(createCluster)

  it must behave like resourceDisabledWhenUnderMaintenance(createCluster)

  "The clusters resource" must "list user clusters" in new WithSampleUsers {
    dao.withConnection { implicit c =>
      val ownCluster = MockedServiceManager.DefaultClusterId
      val otherCluster = ClusterId()
      dao.assignCluster(ownCluster, user1.id)
      dao.assignCluster(otherCluster, user2.id)
      val resource = route(listClusters.authorizedBy(user1)).get
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
      contentAsString(resource) must include (ownCluster.toString)
      contentAsString(resource) must include ("cluster0")
      contentAsString(resource) must not include otherCluster.toString
    }
  }

  it must "start a new cluster if no services are specified" in new WithSampleUsers {
    val resource = route(FakeRequest(POST, resourcePath)
      .withJsonBody(Json.obj("name" -> "cluster_new", "size" -> 10))
      .authorizedBy(user1)).get
    status(resource) must equal (CREATED)
    contentType(resource) must be (Some("application/json"))
    val location = header("Location", resource)
    location must be ('defined)
    contentAsString(resource) must include (location.get)
    dao.withConnection { implicit c =>
      dao.clustersOf(user1.id) must have length 1
    }
  }

  it must "start a new cluster if some services are specified" in new WithSampleUsers {
    val resource = route(createCluster.authorizedBy(user1)).get
    status(resource) must equal (CREATED)
    contentType(resource) must be (Some("application/json"))
    val location = header("Location", resource)
    location must be ('defined)
    contentAsString(resource) must include (location.get)
    dao.withConnection { implicit c =>
      dao.clustersOf(user1.id) must have length 1
    }
  }

  it must "reject cluster creation with invalid payload" in new WithSampleUsers {
    val resource = route(FakeRequest(POST, resourcePath)
      .withJsonBody(inValidCreationParams)
      .authorizedBy(user1)).get
    status(resource) must equal (BAD_REQUEST)
  }
}
