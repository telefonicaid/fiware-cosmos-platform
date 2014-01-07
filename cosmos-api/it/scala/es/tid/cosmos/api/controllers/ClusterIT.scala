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
import org.scalatest.matchers.{Matcher, MatchResult, MustMatchers}
import play.api.libs.json._
import play.api.test.Helpers._
import play.api.test.FakeRequest

import es.tid.cosmos.api.controllers.ResultMatchers.failWith
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.test.matchers.JsonMatchers
import es.tid.cosmos.servicemanager.clusters._

class ClusterIT
  extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors
  with JsonMatchers {

  import MockedServiceManager._

  "Cluster detail listing" must behave like
    rejectingUnauthenticatedRequests(DefaultClusterProps.listDetailsRequest)

  it must behave like enabledWhenUnderMaintenance(DefaultClusterProps.listDetailsRequest)

  "Cluster termination" must behave like
    rejectingUnauthenticatedRequests(DefaultClusterProps.terminateRequest)

  it must behave like
    enabledOnlyForOperatorsWhenUnderMaintenance(DefaultClusterProps.terminateRequest)

  "Cluster user management" must behave like
    rejectingUnauthenticatedRequests(DefaultClusterProps.addUserRequest("pepito"))

  it must behave like
    enabledOnlyForOperatorsWhenUnderMaintenance(DefaultClusterProps.addUserRequest("pepito"))

  "Cluster resource" must "list complete cluster details on GET request when cluster is running" in
    new WithSampleSessions {
      regUser.setAsOwner(DefaultClusterProps.id)
      val resource = regUser.doRequest(DefaultClusterProps.listDetailsRequest)
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
      val description = contentAsJson(resource)
      description must representClusterProperties(DefaultClusterProps)
      description must representRunningCluster
    }

  it must "list partial cluster details on GET request" +
    " when cluster is still provisioning" in new WithSampleSessions {
    regUser.setAsOwner(InProgressClusterProps.id)
    val resource = regUser.doRequest(InProgressClusterProps.listDetailsRequest)
    status(resource) must equal (OK)
    contentType(resource) must be (Some("application/json"))
    val description = contentAsJson(resource)
    description must representClusterProperties(InProgressClusterProps)
    description must representInProgressCluster
  }

  it must "return 404 on unknown cluster" in new WithSampleSessions {
    val resource = regUser.doRequest(UnknownClusterProps.listDetailsRequest)
    status(resource) must equal (NOT_FOUND)
  }

  it must "reject with 401 when listing non-owned cluster" in new WithSampleSessions {
    val resource = regUser.doRequest(DefaultClusterProps.listDetailsRequest)
    status(resource) must equal (UNAUTHORIZED)
  }

  it must "throw if the service manager has no associated information" in new WithSampleSessions {
      regUser.setAsOwner(UnknownClusterProps.id)
      val resource = regUser.doRequest(UnknownClusterProps.listDetailsRequest)
      resource must failWith (classOf[IllegalStateException])
    }

  it must "terminate cluster" in new WithSampleSessions {
    regUser.setAsOwner(DefaultClusterProps.id)
    val resource = regUser.doRequest(DefaultClusterProps.terminateRequest)
    status(resource) must equal (OK)
    val cluster = services.serviceManager().describeCluster(DefaultClusterProps.id).get
    cluster.state must (be (Terminating) or be (Terminated))
  }

  it must "return 404 when terminating unknown clusters" in new WithSampleSessions {
    status(regUser.doRequest(UnknownClusterProps.terminateRequest)) must equal (NOT_FOUND)
  }

  it must "reject cluster termination of non owned clusters" in new WithSampleSessions {
    status(regUser.doRequest(DefaultClusterProps.terminateRequest)) must equal (UNAUTHORIZED)
  }

  it must "be idempotent respect to cluster termination" in new WithSampleSessions {
    regUser.setAsOwner(DefaultClusterProps.id)
    for (_ <- 1 to 2) {
      status(regUser.doRequest(DefaultClusterProps.terminateRequest)) must equal (OK)
      val cluster = services.serviceManager().describeCluster(DefaultClusterProps.id).get
      cluster.state must (be (Terminating) or be (Terminated))
    }
  }

  it must "add a user to cluster" in new WithSampleSessions {
    regUser.setAsOwner(DefaultClusterProps.id)
    val rep = regUser.doRequest(DefaultClusterProps.addUserRequest(opUser.handle))
    status(rep) must equal (OK)
    val users = services.serviceManager().listUsers(DefaultClusterProps.id)
    users must be ('defined)
    users.get.exists(
      usr => usr.username.equals(opUser.handle) && usr.isEnabled
    ) must be (true)
  }

  it must "fail to add an already existing user" in new WithSampleSessions {
    regUser.setAsOwner(DefaultClusterProps.id)
    val rep1 = regUser.doRequest(DefaultClusterProps.addUserRequest(opUser.handle))
    status(rep1) must equal (OK)

    val rep2 = regUser.doRequest(DefaultClusterProps.addUserRequest(opUser.handle))
    status(rep2) must equal (BAD_REQUEST)
  }

  it must "remove a user from cluster" in new WithSampleSessions {
    regUser.setAsOwner(DefaultClusterProps.id)
    status(regUser.doRequest(DefaultClusterProps.addUserRequest(opUser.handle)))

    val rep = regUser.doRequest(DefaultClusterProps.removeUserRequest(opUser.handle))
    status(rep) must equal (OK)
    val users = services.serviceManager().listUsers(DefaultClusterProps.id)
    users must be ('defined)
    users.get.exists(
      usr => usr.username.equals("pocahontas") && usr.isEnabled
    ) must be (false)
  }

  it must "fail to remove the owner of the cluster" in new WithSampleSessions {
    regUser.setAsOwner(DefaultClusterProps.id)
    val rep = regUser.doRequest(DefaultClusterProps.removeUserRequest(regUser.handle))
    status(rep) must equal (BAD_REQUEST)
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

  private implicit class RequestFactory(props: MockedServiceManager.ClusterProperties) {

    val baseUrl = s"/cosmos/v1/cluster/${props.id}"

    def listDetailsRequest = FakeRequest(GET, baseUrl)
    def terminateRequest = FakeRequest(POST, s"$baseUrl/terminate")
    def addUserRequest(handle: String) = FakeRequest(POST, s"$baseUrl/add_user").withJsonBody(
      Json.obj("user" -> handle)
    )
    def removeUserRequest(handle: String) = FakeRequest(POST, s"$baseUrl/remove_user").withJsonBody(
      Json.obj("user" -> handle)
    )
  }
}
