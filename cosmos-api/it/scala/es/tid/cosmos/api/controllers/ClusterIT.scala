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
import org.scalatest.matchers.{Matcher, MatchResult, MustMatchers}
import play.api.libs.json._
import play.api.test.Helpers._
import play.api.test.FakeRequest

import es.tid.cosmos.api.controllers.ResultMatchers.failWith
import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.mocks.SampleClusters
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.test.matchers.JsonMatchers
import es.tid.cosmos.servicemanager.clusters._

class ClusterIT
  extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors
  with JsonMatchers {

  import SampleClusters._

  "Cluster detail listing" must behave like
    rejectingUnauthenticatedRequests(RunningClusterProps.listDetailsRequest)

  it must behave like enabledWhenUnderMaintenance(RunningClusterProps.listDetailsRequest)

  "Cluster termination" must behave like
    rejectingUnauthenticatedRequests(RunningClusterProps.terminateRequest)

  it must behave like
    enabledOnlyForOperatorsWhenUnderMaintenance(RunningClusterProps.terminateRequest)

  "Cluster resource" must "list complete cluster details on GET request when cluster is running" in
    new WithSampleSessions with SampleClusters {
      regUserInGroup.assignCluster(RunningClusterProps.id, shared = false)
      val resource = regUserInGroup.doRequest(RunningClusterProps.listDetailsRequest)
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
      val description = contentAsJson(resource)
      description must representClusterProperties(RunningClusterProps)
      description must representRunningCluster
      description must representNonSharedCluster
    }

  it must "list complete cluster details on GET request when cluster is shared" in
    new WithSampleSessions with SampleClusters {
      regUserInGroup.assignCluster(RunningClusterProps.id, shared = true)
      val resource = regUserInGroup.doRequest(RunningClusterProps.listDetailsRequest)
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
      val description = contentAsJson(resource)
      description must representClusterProperties(RunningClusterProps)
      description must representRunningCluster
      description must representSharedCluster
    }

  it must "list partial cluster details on GET request" +
    " when cluster is still provisioning" in new WithSampleSessions with SampleClusters {
    regUserInGroup.assignCluster(ProvisioningClusterProps.id, shared = false)
    val resource = regUserInGroup.doRequest(ProvisioningClusterProps.listDetailsRequest)
    status(resource) must equal (OK)
    contentType(resource) must be (Some("application/json"))
    val description = contentAsJson(resource)
    description must representClusterProperties(ProvisioningClusterProps)
    description must representInProgressCluster
    description must representNonSharedCluster
  }

  it must "return 404 on unknown cluster" in new WithSampleSessions {
    val resource = regUserInGroup.doRequest(RunningClusterProps.listDetailsRequest)
    status(resource) must equal (NOT_FOUND)
  }

  it must "reject with 401 when listing non-owned cluster" in
      new WithSampleSessions with SampleClusters {
    val resource = regUserInGroup.doRequest(RunningClusterProps.listDetailsRequest)
    status(resource) must equal (UNAUTHORIZED)
  }

  it must "throw if the service manager has no associated information" in new WithSampleSessions {
      regUserInGroup.assignCluster(RunningClusterProps.id, shared = false)
      val resource = regUserInGroup.doRequest(RunningClusterProps.listDetailsRequest)
      resource must failWith (classOf[IllegalStateException])
    }

  it must "terminate cluster" in new WithSampleSessions with SampleClusters {
    regUserInGroup.assignCluster(RunningClusterProps.id, shared = false)
    val resource = regUserInGroup.doRequest(RunningClusterProps.terminateRequest)
    status(resource) must equal (OK)
    val cluster = services.serviceManager.describeCluster(RunningClusterProps.id).get
    cluster.state must (be (Terminating) or be (Terminated))
  }

  it must "return 404 when terminating unknown clusters" in new WithSampleSessions {
    status(regUserInGroup.doRequest(RunningClusterProps.terminateRequest)) must equal (NOT_FOUND)
  }

  it must "reject cluster termination of non owned clusters" in
      new WithSampleSessions with SampleClusters {
    status(regUserInGroup.doRequest(RunningClusterProps.terminateRequest)) must equal (UNAUTHORIZED)
  }

  it must "be idempotent respect to cluster termination" in
      new WithSampleSessions with SampleClusters {
    regUserInGroup.assignCluster(RunningClusterProps.id, shared = false)
    for (_ <- 1 to 2) {
      status(regUserInGroup.doRequest(RunningClusterProps.terminateRequest)) must equal (OK)
      val cluster = services.serviceManager.describeCluster(RunningClusterProps.id).get
      cluster.state must (be (Terminating) or be (Terminated))
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
    containFieldWithValue("id", JsString(props.id.toString)) and
    containFieldWithUrl("href") and
    containFieldWithValue("name", JsString(props.name.toString)) and
    containFieldWithValue("size", JsNumber(props.size)) and
    containFieldWithValue("services", JsArray(props.services.map(JsString)))

  private val representRunningCluster =
    containFieldWithValue("state", JsString(Running.name)) and
    containFieldWithValue("stateDescription", JsString(Running.descLine)) and
    containFieldThatMust("master", representAMachine) and
    containFieldThatMust("slaves", beAnArrayWhoseElementsMust(representAMachine)) and
    containFieldThatMust("users", beAnArrayWhoseElementsMust(representAUser))

  private val representSharedCluster =
    containFieldWithValue("shared", JsBoolean(true))

  private val representNonSharedCluster =
    containFieldWithValue("shared", JsBoolean(false))

  private val representInProgressCluster =
    containFieldWithValue("state", JsString(Provisioning.name)) and
    containFieldWithValue("stateDescription", JsString(Provisioning.descLine))

  private implicit class RequestFactory(props: MockedServiceManager.ClusterProperties) {

    val baseUrl = s"/cosmos/v1/cluster/${props.id}"

    def listDetailsRequest = FakeRequest(GET, baseUrl)
    def terminateRequest = FakeRequest(POST, s"$baseUrl/terminate")
  }
}
