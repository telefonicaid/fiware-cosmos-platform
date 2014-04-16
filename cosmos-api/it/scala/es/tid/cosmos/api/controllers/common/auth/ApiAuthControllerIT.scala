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

package es.tid.cosmos.api.controllers.common.auth

import scala.concurrent.Future
import scala.language.reflectiveCalls
import scalaz.Scalaz

import org.mockito.BDDMockito.given
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import play.api.mvc._
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.auth.request.{AuthError, MissingAuthentication, RequestAuthentication}
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.profile._

class ApiAuthControllerIT extends FlatSpec with MustMatchers with MockitoSugar {

  import Scalaz._

  trait WithTestController extends WithSampleSessions {
    val authentication = mock[RequestAuthentication]

    object TestController extends Controller with ApiAuthController {
      override val auth = authentication

      def index() = Action(parse.anyContent) { request =>
        for {
          profile <- requireAuthenticatedApiRequest(request)
        } yield Ok(s"handle=${profile.handle}")
      }

      def admin() = Action(parse.anyContent) { request =>
        for {
          profile <- requireOperatorApiRequest(request)
        } yield Ok("only for ops")
      }
    }

    val request = FakeRequest(GET, "/")

    def requestIndex(): Future[SimpleResult] = TestController.index().apply(request)
    def requestAdmin(): Future[SimpleResult] = TestController.admin().apply(request)

    def failAuthWith(authError: AuthError) {
      given(authentication.authenticateRequest(request)).willReturn(authError.fail)
    }

    def authenticateProfile(profile: CosmosProfile) {
      given(authentication.authenticateRequest(request)).willReturn(profile.success)
    }
  }

  val registration = Registration("login", "ssh-rsa AAAA login@host", "login@host")

  "The API auth controller" must "return a UNAUTHORIZED response when auth fails" in
    new WithTestController {
      failAuthWith(MissingAuthentication)
      val res = requestIndex()
      status(res) must be (UNAUTHORIZED)
      contentAsString(res) must include ("Missing authorization header")
    }

  it must "authorize valid requests" in new WithTestController {
    authenticateProfile(regUserInGroup.cosmosProfile)
    val res = requestIndex()
    status(res) must be (OK)
    contentAsString(res) must include (s"handle=${regUserInGroup.handle}")
  }

  it must "reject disabled user requests" in new WithTestController {
    authenticateProfile(userWithState(UserState.Disabled).cosmosProfile)
    val res = requestIndex()
    status(res) must be (UNAUTHORIZED)
    contentAsString(res) must include ("User profile is disabled")
  }

  it must "reject non operator from admin pages" in new WithTestController {
    authenticateProfile(regUserInGroup.cosmosProfile)
    val res = requestAdmin()
    status(res) must be (FORBIDDEN)
    contentAsString(res) must include ("You need to be an operator")
  }

  it must "accept operators on admin pages" in new WithTestController {
    authenticateProfile(opUser.cosmosProfile)
    status(requestAdmin()) must be (OK)
  }
}
