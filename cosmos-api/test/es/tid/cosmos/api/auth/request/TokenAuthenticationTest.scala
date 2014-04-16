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

import scala.concurrent.Future
import scalaz._

import org.mockito.BDDMockito.given
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import play.api.test.FakeRequest

import es.tid.cosmos.api.auth.oauth2.{OAuthUserProfile, OAuthProvider}
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.UserState.Enabled
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent

class TokenAuthenticationTest extends FlatSpec with MustMatchers with MockitoSugar {

  import Scalaz._

  trait WithInstance extends MockCosmosDataStoreComponent {
    val oauth = mock[OAuthProvider]
    val token = "oauth-token"
    val oauthProfile = OAuthUserProfile(UserId("id"))
    val requestWithoutToken = FakeRequest("GET", "/resource")
    val requestWithToken = requestWithoutToken.withHeaders("X-Auth-Token" -> token)
    val authentication = new TokenAuthentication(oauth, store)

    def givenTokenIsValid() {
      given(oauth.requestUserProfile(token)).willReturn(Future.successful(oauthProfile))
    }

    def givenTokenIsInvalid() {
      given(oauth.requestUserProfile(token)).willReturn(Future.failed(new Error("Invalid token")))
    }

    def registerUser() = store.withTransaction { implicit c =>
      store.profile.register(
        oauthProfile.id, Registration("handle", "ssh-rsa XXX", "user@host"), Enabled)
    }
  }

  "Token based authentication" must "authenticate valid user tokens" in new WithInstance {
    val cosmosProfile = registerUser()
    givenTokenIsValid()
    authentication.authenticateRequest(requestWithToken) must be (cosmosProfile.success)
  }

  it must "reject OAuth users without Cosmos profile" in new WithInstance {
    givenTokenIsValid()
    authentication.authenticateRequest(requestWithToken) must be (InvalidAuthCredentials.failure)
  }

  it must "reject requests with invalid tokens" in new WithInstance {
    registerUser()
    givenTokenIsInvalid()
    val result = authentication.authenticateRequest(requestWithToken)
    result must be ('failure)
    result.toString must include ("Cannot retrieve user profile")
  }

  it must "reject request without token header" in new WithInstance {
    authentication.authenticateRequest(requestWithoutToken) must be (MissingAuthentication.failure)
  }
}
