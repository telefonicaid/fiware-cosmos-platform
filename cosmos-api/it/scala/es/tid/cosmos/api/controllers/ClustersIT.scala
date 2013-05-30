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
import play.api.db.DB
import play.api.libs.json.{JsValue, Json}
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.clusters.CreateClusterParams
import es.tid.cosmos.api.mocks.WithSampleUsers
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile.CosmosProfileDao
import es.tid.cosmos.servicemanager.ClusterId

class ClustersIT extends FlatSpec with MustMatchers {
  val validCreationParams: JsValue = Json.toJson(CreateClusterParams("cluster_new", 120))
  val inValidCreationParams: JsValue = Json.obj("invalid" -> "json")

  "The clusters resource" must "list user clusters" in new WithSampleUsers {
    DB.withConnection { implicit c =>
      val ownCluster = MockedServiceManager.defaultClusterId
      val otherCluster = ClusterId()
      CosmosProfileDao.assignCluster(ownCluster, user1.id)
      CosmosProfileDao.assignCluster(otherCluster, user2.id)
      val resource = route(FakeRequest(GET, "/cosmos/cluster").authorizedBy(user1)).get
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
      contentAsString(resource) must include (ownCluster.toString)
      contentAsString(resource) must not include (otherCluster.toString)
    }
  }

  it must "reject unauthenticated cluster listing" in new WithSampleUsers {
    val resource = route(FakeRequest(GET, "/cosmos/cluster")).get
    status(resource) must equal (UNAUTHORIZED)
  }

  it must "start a new cluster" in new WithSampleUsers {
    val resource = route(FakeRequest(POST, "/cosmos/cluster")
      .withJsonBody(validCreationParams)
      .authorizedBy(user1)).get
    status(resource) must equal (CREATED)
    contentType(resource) must be (Some("application/json"))
    val location = header("Location", resource)
    location must be ('defined)
    contentAsString(resource) must include (location.get)
    DB.withConnection { implicit c =>
      CosmosProfileDao.clustersOf(user1.id) must have length (1)
    }
  }

  it must "reject unauthenticated cluster creation" in new WithSampleUsers {
    val resource = route(FakeRequest(POST, "/cosmos/cluster")
      .withJsonBody(validCreationParams)).get
    status(resource) must equal (UNAUTHORIZED)
  }

  it must "reject cluster creation with invalid payload" in new WithSampleUsers {
    val resource = route(FakeRequest(POST, "/cosmos/cluster")
      .withJsonBody(inValidCreationParams)
      .authorizedBy(user1)).get
    status(resource) must equal (BAD_REQUEST)
  }
}
