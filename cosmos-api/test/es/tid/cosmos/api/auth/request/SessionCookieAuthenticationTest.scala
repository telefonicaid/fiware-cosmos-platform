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
import play.api.mvc.Session
import play.api.test.{FakeHeaders, FakeRequest}

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent

class SessionCookieAuthenticationTest extends FlatSpec with MustMatchers {

  import Scalaz._

  trait WithInstance extends MockCosmosDataStoreComponent {
    val userId = UserId("id")
    val userProfile = store.withTransaction { implicit c =>
      store.profile.register(
        userId, Registration("user", "ssh-rsa XXXX", "user@host"), UserState.Enabled)
    }
    val auth = new SessionCookieAuthentication(store)

    val requestWithoutSession = FakeRequest("GET", "/sample/path")

    def requestWithSession(userId: UserId) = new FakeRequest(
      "GET", "/sample/path", headers = FakeHeaders(), body = "") {
      override lazy val session: Session = new Session()
        .setUserProfile(OAuthUserProfile(userId))
        .setToken("Token")
    }
  }

  "Session cookie authentication" must "succeed with the user profile" in new WithInstance {
    auth.authenticateRequest(requestWithSession(userId)) must be (userProfile.success)
  }

  it must "fail with missing auth error when there is no session" in
    new WithInstance {
      auth.authenticateRequest(requestWithoutSession) must be (MissingAuthentication.fail)
    }

  it must "fail with missing auth error when session correspond to an unknown user" in
    new WithInstance {
      val request = requestWithSession(UserId("unknown"))
      auth.authenticateRequest(request) must be (MissingAuthentication.fail)
    }
}
