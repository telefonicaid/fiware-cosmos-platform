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
    val response = regUserInGroup.doRequest(path)
    status(response) must be (FORBIDDEN)
    contentAsString(response) must include ("You are not authorized")
  }

  it must "show the maintenance status" in new WithSampleSessions {
    val response = opUser.doRequest(path)
    status(response) must be (OK)
    contentAsString(response) must include ("Maintenance mode")
  }
}
