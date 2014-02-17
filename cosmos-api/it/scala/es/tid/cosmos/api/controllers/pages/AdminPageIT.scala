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

import scala.language.reflectiveCalls

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.{MaintenanceModeBehaviors, AuthBehaviors}
import es.tid.cosmos.api.controllers.ResultMatchers.redirectTo

class AdminPageIT
  extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors {

  val path = "/admin"

  "The admin page" must behave like enabledWhenUnderMaintenance(FakeRequest(GET, path))

  it must "redirect not registered users" in new WithSampleSessions {
    unregUser.doRequest(path) must redirectTo ("/register")
    unauthUser.doRequest(path) must redirectTo ("/")
  }

  it must "be not found by users without operator capability" in new WithSampleSessions {
    val response = regUser.doRequest(path)
    status(response) must be (FORBIDDEN)
    contentAsString(response) must include ("You are not authorized")
  }

  it must "show the maintenance status" in new WithSampleSessions {
    val response = opUser.doRequest(path)
    status(response) must be (OK)
    contentAsString(response) must include ("Maintenance mode")
  }
}
