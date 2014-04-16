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

package es.tid.cosmos.api.controllers.pages

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.mvc.Session

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.profile.UserId

class CosmosSessionTest extends FlatSpec with MustMatchers {
  val profile = OAuthUserProfile(id=UserId("db-1234"), name = Some("John Smith"))

  trait WithEmptySession {
    def session: CosmosSession = new Session
  }

  trait WithUserSession {
    def session: CosmosSession = Session().setUserProfile(profile)
  }

  "An empty cosmos session" must "be not authenticated" in new WithEmptySession {
    session must not be 'authenticated
  }

  it must "store and retrieve the OAuth token" in new WithEmptySession {
    session.setToken("token").token must be (Some("token"))
  }

  it must "store and retrieve the user profile" in new WithEmptySession {
    session.setUserProfile(profile).userProfile must be (Some(profile))
  }

  "A cosmos session with a user id but no cosmos id" must "be authenticated" in
    new WithUserSession {
      session must be ('authenticated)
    }
}
