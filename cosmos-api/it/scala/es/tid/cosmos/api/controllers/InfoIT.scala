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

package es.tid.cosmos.api.controllers

import org.scalatest.FlatSpec
import org.scalatest.concurrent.Eventually
import org.scalatest.matchers.MustMatchers
import org.scalatest.time.{Millis, Seconds, Span}
import play.api.libs.json.{Json, JsString, JsNumber}
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager.ClusterProperties
import es.tid.cosmos.api.profile.CosmosProfile
import es.tid.cosmos.api.test.matchers.JsonMatchers
import es.tid.cosmos.api.quota.{Quota, GuaranteedGroup}
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.ClusterName
import es.tid.cosmos.servicemanager.clusters._

class InfoIT extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors
  with JsonMatchers with Eventually with FutureMatchers {

  override implicit def patienceConfig =
    PatienceConfig(timeout = Span(5, Seconds), interval = Span(100, Millis))

  val getInfo = FakeRequest(GET, "/cosmos/v1/info")

  "A general info request" must behave like rejectingUnauthenticatedRequests(getInfo)

  it must behave like enabledWhenUnderMaintenance(getInfo)

  it must "provide profile information about id, handle and individual quota" in
    new WithSampleSessions {
      val res = regUserNoGroup.doRequest(getInfo)
      status(res) must be (OK)
      val quota = regUserNoGroup.cosmosProfile.quota.toOptInt.get
      contentAsJson(res) must (
        containFieldWithValue("profileId", JsNumber(regUserNoGroup.cosmosProfile.id)) and
        containFieldWithValue("handle", JsString(regUserNoGroup.handle)) and
        containFieldWithValue("individualQuota", JsNumber(quota))
      )
    }

  it must "provide group name and group quota" in new WithSampleSessions {
    val group = regUserInGroup.group
    val res = regUserInGroup.doRequest(getInfo)
    status(res) must be (OK)
    contentAsJson(res) must containFieldThatMust("group",
      containFieldWithValue("name", JsString(group.name)) and
      containFieldWithValue("guaranteedQuota", JsNumber(group.minimumQuota.toInt))
    )
  }

  it must "provide info about clusters owned or accessible by SSH" in
    new WithSampleSessions {
      val cluster1 = mockedServiceManager.createCluster(
        name = ClusterName("ownCluster"),
        size = 2,
        serviceInstances = Set.empty,
        users = Seq.empty
      )
      regUserInGroup.assignCluster(cluster1, shared = false)
      val cluster2 = ClusterId("cluster2")
      mockedServiceManager.defineCluster(ClusterProperties(
        id = cluster2,
        name = ClusterName("cluster2"),
        users = Set(opUser.asClusterUser(), regUserInGroup.asClusterUser()),
        size  = 2,
        initialState = Some(Running)
      ))
      mockedServiceManager.createCluster(
        name = ClusterName("unlisted"),
        size = 2,
        serviceInstances = Set.empty,
        users = Seq(opUser.asClusterUser(), regUserInGroup.asClusterUser(sshEnabled = false))
      )

      val res = regUserInGroup.doRequest(getInfo)
      status(res) must be (OK)

      contentAsJson(res) must containFieldThatMust("clusters",
        containFieldWithValue("owned", Json.arr(cluster1.toString)) and
        containFieldWithValue("accessible", Json.arr(cluster2.toString))
      )
    }

  it must "filter terminated clusters from the owned listing" in
    new WithSampleSessions {
      val cluster1 = mockedServiceManager.defineCluster(MockedServiceManager.ClusterProperties(
        id = ClusterId.random(),
        name = ClusterName("own but terminated"),
        size = 10,
        users = Set(regUserInGroup.asClusterUser()),
        initialState = Some(Terminated)
      ))
      regUserInGroup.assignCluster(cluster1.view.id, shared = false)

      val res = regUserInGroup.doRequest(getInfo)
      status(res) must be (OK)

      contentAsJson(res) must containFieldThatMust("clusters",
        containFieldWithValue("owned", Json.arr())
      )
    }

  it must "filter non-running clusters from the accessible listing" in
    new WithSampleSessions {
      val cluster1 = mockedServiceManager.defineCluster(ClusterProperties(
        id = ClusterId("cluster1"),
        name = ClusterName("added to but not running"),
        users = Set(opUser.asClusterUser(), regUserInGroup.asClusterUser()),
        size  = 2,
        initialState = Some(Provisioning)
      ))
      opUser.assignCluster(cluster1.view.id, shared = false)

      val res = regUserInGroup.doRequest(getInfo)
      status(res) must be (OK)

      contentAsJson(res) must containFieldThatMust("clusters",
        containFieldWithValue("accessible", Json.arr())
      )
    }

  it must "provide info about existing and available resources" in new WithSampleSessions {
    val guaranteedGroup = GuaranteedGroup("fooGroup", Quota(10))
    store.withTransaction { implicit c =>
      store.group.register(guaranteedGroup)
    }
    val fooGroupUser = new RegisteredUserSession("user", "Mr. user", guaranteedGroup)
    val cluster = mockedServiceManager.createCluster(
      name = ClusterName("ownCluster"),
      size = 2,
      serviceInstances = Set.empty,
      users = Seq.empty
    )
    fooGroupUser.assignCluster(cluster, shared = false)
    val res = fooGroupUser.doRequest(getInfo)
    status(res) must be (OK)
    val quota = fooGroupUser.cosmosProfile.quota.toOptInt.get
    contentAsJson(res) must containFieldThatMust("resources",
      containFieldWithValue("groupConsumption", JsNumber(2)) and
      containFieldWithValue("individualConsumption", JsNumber(2)) and
      containFieldWithValue("available",
        JsNumber(mockedServiceManager.clusterNodePoolCount - 2)) and
      containFieldWithValue("availableForGroup",
        JsNumber(mockedServiceManager.clusterNodePoolCount - 5 - 2)) and
      containFieldWithValue("availableForUser",
        JsNumber(CosmosProfile.DefaultQuota.toOptInt.get - 2))
    )
  }
}
