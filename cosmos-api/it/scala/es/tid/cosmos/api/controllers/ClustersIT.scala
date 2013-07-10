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

class ClustersIT extends FlatSpec with MustMatchers with AuthBehaviors {

  val validCreationParams: JsValue = Json.toJson(CreateClusterParams("cluster_new", 120))
  val inValidCreationParams: JsValue = Json.obj("invalid" -> "json")
  val resourcePath = "/cosmos/v1/cluster"

  "Cluster listing" must behave like
    rejectingUnauthenticatedRequests(FakeRequest(GET, resourcePath))

  "Cluster creation" must behave like rejectingUnauthenticatedRequests(
    FakeRequest(POST, resourcePath).withJsonBody(validCreationParams))

  "The clusters resource" must "list user clusters" in new WithSampleUsers {
    DB.withConnection { implicit c =>
      val ownCluster = MockedServiceManager.DefaultClusterId
      val otherCluster = ClusterId()
      CosmosProfileDao.assignCluster(ownCluster, user1.id)
      CosmosProfileDao.assignCluster(otherCluster, user2.id)
      val resource = route(FakeRequest(GET, resourcePath).authorizedBy(user1)).get
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
      contentAsString(resource) must include (ownCluster.toString)
      contentAsString(resource) must include ("cluster0")
      contentAsString(resource) must not include (otherCluster.toString)
    }
  }

  it must "start a new cluster" in new WithSampleUsers {
    val resource = route(FakeRequest(POST, resourcePath)
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

  it must "detect quota exceeded" in new WithSampleUsers {
    val resource = route(FakeRequest(POST, resourcePath)
      .withJsonBody(validCreationParams)
      .authorizedBy(user3)).get
    status(resource) must equal (FORBIDDEN)
  }

  it must "reject cluster creation with invalid payload" in new WithSampleUsers {
    val resource = route(FakeRequest(POST, resourcePath)
      .withJsonBody(inValidCreationParams)
      .authorizedBy(user1)).get
    status(resource) must equal (BAD_REQUEST)
  }
}
