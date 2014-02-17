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

package es.tid.cosmos.api.controllers.pages

import scala.concurrent.Future

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test.Helpers._
import play.api.mvc._

import es.tid.cosmos.api.controllers.common.PagesAuthController
import es.tid.cosmos.api.profile.{UserState, CosmosProfileDao}

class PagesAuthControllerIT extends FlatSpec with MustMatchers {

  class TestController(override val dao: CosmosProfileDao)
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
    val controller = new TestController(dao)
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
    contentAsString(route(regUser.request("/"))) must include ("registered")
  }

  it must "discard sessions of not-enabled users" in new WithTestController {
    contentAsString(route(userWithState(UserState.Disabled).request("/"))) must include (
      "not registered")
  }
}
