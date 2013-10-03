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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test.Helpers._
import play.api.mvc.{AnyContent, Result, Request, Action}

import es.tid.cosmos.api.profile.CosmosProfileDao

class PagesAuthControllerIT extends FlatSpec with MustMatchers {

  class TestController(override val dao: CosmosProfileDao) extends PagesAuthController {
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
    def route(request: Request[AnyContent]): Result = controller.index().apply(request)
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
}
