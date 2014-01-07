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
import scala.concurrent.duration._

import org.scalatest.FlatSpec
import org.scalatest.matchers.{Matcher, MatchResult, MustMatchers}
import play.api.libs.json._
import play.api.test.Helpers._
import play.api.test.FakeRequest

import es.tid.cosmos.api.controllers.ResultMatchers.failWith
import es.tid.cosmos.api.mocks.WithSampleUsers
import es.tid.cosmos.api.mocks.servicemanager.{MockedServiceManagerComponent, MockedServiceManager}
import es.tid.cosmos.api.test.matchers.JsonMatchers
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.ClusterUser
import scala.concurrent.Await

class ClusterIT
  extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors
  with JsonMatchers {

  import MockedServiceManager.DefaultClusterProps
  import MockedServiceManager.InProgressClusterProps

  val runningClusterPath = s"/cosmos/v1/cluster/${DefaultClusterProps.id}"
  val provisioningClusterPath = s"/cosmos/v1/cluster/${InProgressClusterProps.id}"
  val unknownClusterId = ClusterId()
  val unknownClusterPath = s"/cosmos/v1/cluster/$unknownClusterId"

  val clusterDetailsListing = FakeRequest(GET, runningClusterPath)
  val clusterTermination = FakeRequest(POST, s"$unknownClusterPath/terminate")

  "Cluster detail listing" must behave like
    rejectingUnauthenticatedRequests(clusterDetailsListing)

  it must behave like enabledWhenUnderMaintenance(clusterDetailsListing)

  "Cluster termination" must behave like
    rejectingUnauthenticatedRequests(clusterTermination)

  it must behave like enabledOnlyForOperatorsWhenUnderMaintenance(clusterTermination)

  "Cluster resource" must "list complete cluster details on GET request when cluster is running" in
    new WithSampleUsers {
      dao.withConnection { implicit c =>
        dao.assignCluster(DefaultClusterProps.id, user1.id)
        Thread.sleep(2 * MockedServiceManagerComponent.TransitionDelay.toMillis)
        val resource = route(clusterDetailsListing.authorizedBy(user1)).get
        status(resource) must equal (OK)
        contentType(resource) must be (Some("application/json"))
        val description = contentAsJson(resource)
        description must representClusterProperties(DefaultClusterProps)
        description must representRunningCluster
      }
    }

  it must "list complete cluster details for users with SSH access" in new WithSampleUsers {
    dao.withConnection { implicit c =>
      val sm = services.serviceManager()
      val clusterUser1 = ClusterUser.enabled(
        username = user2.handle,
        publicKey = user2.keys(0).signature
      )
      dao.assignCluster(DefaultClusterProps.id, user1.id)
      Thread.sleep(2 * MockedServiceManagerComponent.TransitionDelay.toMillis)
      Await.ready(
        sm.setUsers(DefaultClusterProps.id, sm.listUsers(DefaultClusterProps.id).get :+ clusterUser1),
        2 seconds
      )
      val resource = route(clusterDetailsListing.authorizedBy(user2)).get
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
      val description = Json.parse(contentAsString(resource))
      description must representClusterProperties(DefaultClusterProps.copy(
        users = DefaultClusterProps.users + clusterUser1
      ))
    }
  }

  it must "list partial cluster details on GET request" +
    " when cluster is still provisioning" in new WithSampleUsers {
    dao.withConnection { implicit c =>
      dao.assignCluster(InProgressClusterProps.id, user1.id)
      val resource = route(FakeRequest(GET, provisioningClusterPath).authorizedBy(user1)).get
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
      val description = Json.parse(contentAsString(resource))
      description must representClusterProperties(InProgressClusterProps)
      description must representInProgressCluster
    }
  }

  it must "return 404 on unknown cluster" in new WithSampleUsers {
    val resource = route(FakeRequest(GET, unknownClusterPath).authorizedBy(user1)).get
    status(resource) must equal (NOT_FOUND)
  }

  it must "reject with 401 when listing non-owned cluster" in new WithSampleUsers {
    val resource = route(clusterDetailsListing.authorizedBy(user2)).get
    status(resource) must equal (UNAUTHORIZED)
  }

  it must "reject with 401 when listing a cluster where the user was removed" in new WithSampleUsers {
    dao.withConnection { implicit c =>
      dao.assignCluster(DefaultClusterProps.id, user1.id)
      Thread.sleep(2 * MockedServiceManagerComponent.TransitionDelay.toMillis)
      val resource = route(clusterDetailsListing.authorizedBy(user3)).get
      status(resource) must equal (UNAUTHORIZED)
    }
  }

  it must "throw if the service manager has no associated information" in
    new WithSampleUsers {
      val clusterId = ClusterId()
      dao.withConnection { implicit c =>
        dao.assignCluster(clusterId, user1.id)
      }
      val resource = route(FakeRequest(GET, s"/cosmos/v1/cluster/$clusterId").authorizedBy(user1)).get
      resource must failWith (classOf[IllegalStateException])
    }

  it must "terminate cluster" in new WithSampleUsers {
    dao.withConnection { implicit c =>
      val clusterId = DefaultClusterProps.id
      dao.assignCluster(clusterId, user1.id)
      val resource = route(FakeRequest(POST, s"$runningClusterPath/terminate").authorizedBy(user1)).get
      status(resource) must equal (OK)
      val cluster = services.serviceManager().describeCluster(clusterId).get
      cluster.state must (be (Terminating) or be (Terminated))
    }
  }

  it must "return 404 when terminating unknown clusters" in new WithSampleUsers {
    status(route(clusterTermination.authorizedBy(user1)).get) must equal (NOT_FOUND)
  }

  it must "reject cluster termination of non owned clusters" in new WithSampleUsers {
    val resource = route(FakeRequest(POST, s"$runningClusterPath/terminate").authorizedBy(user1)).get
    status(resource) must equal (UNAUTHORIZED)
  }

  it must "be idempotent respect to cluster termination" in new WithSampleUsers {
    dao.withConnection { implicit c =>
      val clusterId = DefaultClusterProps.id
      dao.assignCluster(clusterId, user1.id)
      val terminateRequest= FakeRequest(POST, s"$runningClusterPath/terminate").authorizedBy(user1)
      for (_ <- 1 to 2) {
        status(route(terminateRequest).get) must equal (OK)
        val cluster = services.serviceManager().describeCluster(clusterId).get
        cluster.state must (be (Terminating) or be (Terminated))
      }
    }
  }

  private object representAMachine extends Matcher[JsObject] {
    def apply(js: JsObject) = MatchResult(
      matches = js match {
        case JsObject(Seq(("hostname", _), ("ipAddress", _))) => true
        case _ => false
      },
      failureMessage = s"$js does not represent a valid machine",
      negatedFailureMessage = s"$js represents a valid machine"
    )
  }

  private object representAUser extends Matcher[JsObject] {
    def apply(js: JsObject) = MatchResult(
      matches = js match {
        case JsObject(Seq(("username", _), ("sshPublicKey", _), ("isSudoer", _))) => true
        case _ => false
      },
      failureMessage = s"$js does not represent a valid user",
      negatedFailureMessage = s"$js represents a valid user"
    )
  }

  private def representClusterProperties(props: MockedServiceManager.ClusterProperties) =
    containsFieldWithValue("id", JsString(props.id.toString)) and
      containsFieldWithUrl("href") and
      containsFieldWithValue("name", JsString(props.name.toString)) and
      containsFieldWithValue("size", JsNumber(props.size)) and
      containsFieldWithValue("services", JsArray(props.services.map(JsString)))

  private val representRunningCluster =
    containsFieldWithValue("state", JsString(Running.name)) and
    containsFieldWithValue("stateDescription", JsString(Running.descLine)) and
    containsFieldThatMust("master", representAMachine) and
    containsFieldThatMust("slaves", beAnArrayWhoseElementsMust(representAMachine)) and
    containsFieldThatMust("users", beAnArrayWhoseElementsMust(representAUser))

  private val representInProgressCluster =
    containsFieldWithValue("state", JsString(Provisioning.name)) and
    containsFieldWithValue("stateDescription", JsString(Provisioning.descLine))
}
