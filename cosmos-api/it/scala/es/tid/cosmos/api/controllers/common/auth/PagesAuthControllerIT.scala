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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test.Helpers._
import play.api.mvc._

import es.tid.cosmos.api.profile.UserState
import es.tid.cosmos.api.profile.dao.CosmosDataStore
import es.tid.cosmos.api.controllers.pages.WithSampleSessions

class PagesAuthControllerIT extends FlatSpec with MustMatchers {

  class TestController(override val store: CosmosDataStore)
    extends Controller with PagesAuthController {

    def index() = Action(parse.anyContent) { request =>
      withAuthentication(request) (
        whenRegistered = (u, c) => Ok(s"registered: userProfile=$u, cosmosProfile=$c"),
        whenNotRegistered = u => Ok(s"not registered: userProfile=$u"),
        whenNotAuthenticated = Ok("not authenticated")
      )
    }
  }

  trait WithTestController extends WithSampleSessions {
    val controller = new TestController(store)
    def route(request: Request[AnyContent]): Future[SimpleResult] =
      controller.index().apply(request)
  }

  "The pages auth controller" must "detect not authenticated sessions" in new WithTestController {
    contentAsString(route(unauthUser.request("/"))) must include ("not authenticated")
  }

  it must "detect sessions of not registered users" in new WithTestController {
    contentAsString(route(unregUser.request("/"))) must include ("not registered")
  }

  it must "detect sessions of registered users" in new WithTestController {
    contentAsString(route(regUserInGroup.request("/"))) must include ("registered")
  }

  it must "discard sessions of not-enabled users" in new WithTestController {
    contentAsString(route(userWithState(UserState.Disabled).request("/"))) must include (
      "not registered")
  }
}
