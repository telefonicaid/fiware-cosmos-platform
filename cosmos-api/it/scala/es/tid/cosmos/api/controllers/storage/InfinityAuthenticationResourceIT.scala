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

package es.tid.cosmos.api.controllers.storage

import scala.concurrent.Future

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.JsString
import play.api.mvc.SimpleResult
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager.ClusterProperties
import es.tid.cosmos.api.profile.{ClusterSecret, ApiCredentials}
import es.tid.cosmos.servicemanager.ClusterName
import es.tid.cosmos.servicemanager.clusters.{Running, ClusterId}

class InfinityAuthenticationResourceIT extends FlatSpec with MustMatchers {

  trait WithFixture extends WithSampleSessions {

    def request(
        apiKey: Option[String] = None,
        apiSecret: Option[String] = None,
        clusterSecret: Option[String] = None): Future[SimpleResult] = {
      val queryString = Seq(
        apiKey.map("apiKey=" + _),
        apiSecret.map("apiSecret=" + _),
        clusterSecret.map("clusterSecret=" + _)
      ).flatten.mkString("?", "&", "")
      route(FakeRequest(GET, "/cosmos/v1/storage/auth" + queryString)).get
    }

    def request(creds: ApiCredentials): Future[SimpleResult] =
      request(apiKey = Some(creds.apiKey), apiSecret = Some(creds.apiSecret))

    def request(secret: ClusterSecret): Future[SimpleResult] =
      request(clusterSecret = Some(secret.underlying))
  }

  "Infinity authentication" must "provide an identity from API credentials" in new WithFixture {
    val res = request(regUserNoGroup.cosmosProfile.apiCredentials)
    status(res) must be (OK)
    contentAsJson(res) \ "user" must be (JsString(regUserNoGroup.handle))
  }

  it must "reject unknown API credentials" in new WithFixture {
    status(request(ApiCredentials.random())) must be (BAD_REQUEST)
  }

  it must "provide an identity from a cluster secret" in new WithFixture {
    val cluster = mockedServiceManager.defineCluster(ClusterProperties(
      id = new ClusterId("someCluster"),
      name = ClusterName("Some cluster"),
      size = 4,
      users = Set(regUserNoGroup.asClusterUser()),
      initialState = Some(Running)
    ))
    val clusterSecret = store.withTransaction { implicit c =>
      store.cluster.register(
        cluster.view.id, regUserNoGroup.cosmosProfile.id, ClusterSecret.random())
    }.secret.get
    val res = request(clusterSecret)
    status(res) must be (OK)
    contentAsJson(res) \ "user" must be (JsString(regUserNoGroup.handle))
  }

  it must "reject unknown cluster secret" in new WithFixture {
    status(request(ClusterSecret.random())) must be (BAD_REQUEST)
  }

  it must "reject request with both API credentials and cluster secret" in new WithFixture {
    val res = request(apiKey= Some("XXX"), apiSecret = Some("YYY"), clusterSecret = Some("ZZZ"))
    status(res) must be (BAD_REQUEST)
    contentAsString(res) must include ("Invalid parameters")
  }

  it must "reject request with missing parameters" in new WithFixture {
    val res = request()
    status(res) must be (BAD_REQUEST)
    contentAsString(res) must include ("Invalid parameters")
  }

  it must "reject request with malformed API credentials" in new WithFixture {
    val res = request(apiKey = Some("malformed"), apiSecret = Some("malformed"))
    status(res) must be (BAD_REQUEST)
    contentAsString(res) must include ("API key must be an alphanumeric sequence of length 20")
  }

  it must "reject request with malformed cluster secret" in new WithFixture {
    val res = request(clusterSecret = Some("malformed"))
    status(res) must be (BAD_REQUEST)
    contentAsString(res) must
      include ("Cluster secret must be an alphanumeric sequence of length 128")
  }
}
