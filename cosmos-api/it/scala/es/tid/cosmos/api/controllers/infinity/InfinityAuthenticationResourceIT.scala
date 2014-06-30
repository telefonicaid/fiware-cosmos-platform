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

package es.tid.cosmos.api.controllers.infinity

import scala.concurrent.Future

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.JsString
import play.api.mvc.SimpleResult
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager.ClusterProperties
import es.tid.cosmos.api.profile.{ApiCredentials, ClusterSecret}
import es.tid.cosmos.common.BearerToken
import es.tid.cosmos.servicemanager.ClusterName
import es.tid.cosmos.servicemanager.clusters.{ClusterId, Running}

class InfinityAuthenticationResourceIT extends FlatSpec with MustMatchers {

  trait Fixture extends WithSampleSessions {

    private val authHeader = "Authorization" -> BearerToken("infinitypass")

    def request(
        apiKey: Option[String] = None,
        apiSecret: Option[String] = None,
        clusterSecret: Option[String] = None,
        authenticated: Boolean = true): Future[SimpleResult] = {
      val queryString = Seq(
        apiKey.map("apiKey=" + _),
        apiSecret.map("apiSecret=" + _),
        clusterSecret.map("clusterSecret=" + _)
      ).flatten.mkString("?", "&", "")
      val baseRequest = FakeRequest(GET, "/infinity/v1/auth" + queryString)
      route(if (authenticated) baseRequest.withHeaders(authHeader) else baseRequest).get
    }

    def request(creds: ApiCredentials): Future[SimpleResult] =
      request(apiKey = Some(creds.apiKey), apiSecret = Some(creds.apiSecret))

    def request(secret: ClusterSecret): Future[SimpleResult] =
      request(clusterSecret = Some(secret.underlying))
  }

  "Infinity authentication" must "be rejected when not authenticated" in new Fixture {
    status(request(authenticated = false)) must be (UNAUTHORIZED)
  }

  it must "provide an identity from API credentials" in new Fixture {
    val res = request(regUserNoGroup.cosmosProfile.apiCredentials)
    status(res) must be (OK)
    contentAsJson(res) \ "user" must be (JsString(regUserNoGroup.handle))
  }

  it must "reject unknown API credentials" in new Fixture {
    status(request(ApiCredentials.random())) must be (NOT_FOUND)
  }

  it must "provide an identity from a cluster secret" in new Fixture {
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

  it must "reject unknown cluster secret" in new Fixture {
    status(request(ClusterSecret.random())) must be (NOT_FOUND)
  }

  it must "reject request with both API credentials and cluster secret" in new Fixture {
    val res = request(apiKey= Some("XXX"), apiSecret = Some("YYY"), clusterSecret = Some("ZZZ"))
    status(res) must be (BAD_REQUEST)
    contentAsString(res) must include ("Invalid parameters")
  }

  it must "reject request with missing parameters" in new Fixture {
    val res = request()
    status(res) must be (BAD_REQUEST)
    contentAsString(res) must include ("Invalid parameters")
  }

  it must "reject request with malformed API credentials" in new Fixture {
    val res = request(apiKey = Some("malformed"), apiSecret = Some("malformed"))
    status(res) must be (BAD_REQUEST)
    contentAsString(res) must include ("API key must be an alphanumeric sequence of length 20")
  }

  it must "reject request with malformed cluster secret" in new Fixture {
    val res = request(clusterSecret = Some("malformed"))
    status(res) must be (BAD_REQUEST)
    contentAsString(res) must
      include ("Cluster secret must be an alphanumeric sequence of length 128")
  }
}
