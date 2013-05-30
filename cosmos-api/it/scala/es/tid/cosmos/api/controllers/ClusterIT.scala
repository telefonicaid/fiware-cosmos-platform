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

import scala.Some

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.db.DB
import play.api.libs.json.Json
import play.api.test.Helpers._
import play.api.test.FakeRequest

import es.tid.cosmos.api.controllers.ResultMatchers.failWith
import es.tid.cosmos.servicemanager.{Terminating, Terminated, ClusterId}
import es.tid.cosmos.api.mocks.WithSampleUsers
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile.CosmosProfileDao

class ClusterIT extends FlatSpec with MustMatchers {
  val resourcePath = s"/cosmos/cluster/${MockedServiceManager.defaultClusterId.toString}"
  val unknownClusterId = ClusterId()
  val unknownResourcePath = s"/cosmos/cluster/$unknownClusterId"

  "Cluster resource" must "list cluster details on GET request" in new WithSampleUsers {
    CosmosProfileDao.assignCluster(
      MockedServiceManager.defaultClusterId, user1.id)(DB.getConnection())
    val resource = route(FakeRequest(GET, resourcePath).authorizedBy(user1)).get
    status(resource) must equal (OK)
    contentType(resource) must be (Some("application/json"))
    val description = Json.parse(contentAsString(resource))
    (description \ "id").as[String] must equal (MockedServiceManager.defaultClusterId.toString)
  }

  it must "reject unauthorized detail listing" in new WithSampleUsers {
    val resource = route(FakeRequest(GET, resourcePath)).get
    status(resource) must equal (UNAUTHORIZED)
  }

  it must "return 404 on unknown cluster" in new WithSampleUsers {
    val resource = route(FakeRequest(GET, unknownResourcePath).authorizedBy(user1)).get
    status(resource) must equal (NOT_FOUND)
  }

  it must "reject with 401 when listing non-owned cluster" in new WithSampleUsers {
    val resource = route(FakeRequest(GET, resourcePath).authorizedBy(user2)).get
    status(resource) must equal (UNAUTHORIZED)
  }

  it must "throw if the service manager has no associated information" in
    new WithSampleUsers {
      val clusterId = ClusterId()
      CosmosProfileDao.assignCluster(clusterId, user1.id)(DB.getConnection())
      val resource = route(FakeRequest(GET, s"/cosmos/cluster/$clusterId").authorizedBy(user1)).get
      resource must failWith (classOf[IllegalStateException])
    }

  it must "terminate cluster" in new WithSampleUsers {
    DB.withConnection { implicit c =>
      val clusterId = MockedServiceManager.defaultClusterId
      CosmosProfileDao.assignCluster(clusterId, user1.id)
      val resource = route(FakeRequest(POST, s"$resourcePath/terminate").authorizedBy(user1)).get
      status(resource) must equal (OK)
      val cluster = services.serviceManager().describeCluster(clusterId).get
      cluster.state must (be (Terminating) or be (Terminated))
    }
  }

  it must "return 404 when terminating unknown clusters" in new WithSampleUsers {
    val resource = route(FakeRequest(POST, s"$unknownResourcePath/terminate")
      .authorizedBy(user1)).get
    status(resource) must equal (NOT_FOUND)
  }

  it must "reject non-authenticated cluster termination" in new WithSampleUsers {
    val resource = route(FakeRequest(POST, s"$unknownResourcePath/terminate")).get
    status(resource) must equal (UNAUTHORIZED)
  }

  it must "reject cluster termination of non owned clusters" in new WithSampleUsers {
    val resource = route(FakeRequest(POST, s"$resourcePath/terminate").authorizedBy(user1)).get
    status(resource) must equal (UNAUTHORIZED)
  }

  it must "be idempotent respect to cluster termination" in new WithSampleUsers {
    DB.withConnection { implicit c =>
      val clusterId = MockedServiceManager.defaultClusterId
      CosmosProfileDao.assignCluster(clusterId, user1.id)
      val terminateRequest= FakeRequest(POST, s"$resourcePath/terminate").authorizedBy(user1)
      for (_ <- 1 to 2) {
        status(route(terminateRequest).get) must equal (OK)
        val cluster = services.serviceManager().describeCluster(clusterId).get
        cluster.state must (be (Terminating) or be (Terminated))
      }
    }
  }
}
