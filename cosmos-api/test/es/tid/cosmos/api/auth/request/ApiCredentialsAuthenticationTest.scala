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

package es.tid.cosmos.api.auth.request

import scalaz._

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test.FakeRequest

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.UserState.Enabled
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.common.BasicAuth

class ApiCredentialsAuthenticationTest extends FlatSpec with MustMatchers {

  import Scalaz._

  trait WithInstance extends MockCosmosDataStoreComponent {
    val auth = new ApiCredentialsAuthentication(store)

    val requestWithoutCredentials = FakeRequest("GET", "/sample/resource")
    def requestWithCredentials(apiKey: String, apiSecret: String) =
      requestWithoutCredentials.withHeaders("Authorization" -> BasicAuth(apiKey, apiSecret))

    val userProfile = store.withTransaction { implicit c =>
      store.profile.register(
        UserId("id"), Registration("user", "ssh-rsa XXX", "user@host"), Enabled)
    }
    val validApiKey = userProfile.apiCredentials.apiKey
    val invalidApiKey = stringOf(ApiCredentials.ApiKeyLength + 1)
    val validApiSecret = userProfile.apiCredentials.apiSecret
    val invalidApiSecret = stringOf(ApiCredentials.ApiSecretLength / 2)

    private def stringOf(size: Int) = Seq.fill(size)("X").mkString
  }

  "API credentials authentication" must "authenticate a valid request" in new WithInstance {
    val request = requestWithCredentials(
      userProfile.apiCredentials.apiKey, userProfile.apiCredentials.apiSecret)
    auth.authenticateRequest(request) must be (userProfile.success)
  }

  it must "fail if there is no 'Authorization' header" in new WithInstance {
    auth.authenticateRequest(requestWithoutCredentials) must be (MissingAuthentication.fail)
  }

  it must "fail if the auth header is malformed" in new WithInstance {
    val requestWithMalformedAuth = FakeRequest("GET", "/sample/resource").withHeaders(
      "Authorization" -> "malformed"
    )
    auth.authenticateRequest(requestWithMalformedAuth) must
      be (MalformedAuthHeader("malformed").fail)
  }

  it must "consider malformed header if keys have wrong lengths" in new WithInstance {
    for (invalidRequest <- Seq(
      requestWithCredentials(invalidApiKey, validApiSecret),
      requestWithCredentials(validApiKey, invalidApiSecret)
    )) {
      auth.authenticateRequest(invalidRequest) must be ('failure)
    }
  }

  it must "fail if the credentials doesn't belong to any user" in new WithInstance {
    val unknownCredentials = ApiCredentials.random()
    val invalidRequest =
      requestWithCredentials(unknownCredentials.apiKey, unknownCredentials.apiSecret)
    auth.authenticateRequest(invalidRequest) must be (InvalidAuthCredentials.fail)
  }
}
