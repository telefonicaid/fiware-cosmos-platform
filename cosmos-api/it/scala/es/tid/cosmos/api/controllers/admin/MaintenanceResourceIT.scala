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

package es.tid.cosmos.api.controllers.admin

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.{MaintenanceModeBehaviors, AuthBehaviors}
import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import play.api.libs.json.JsBoolean

class MaintenanceResourceIT
  extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors {

  val path = "/cosmos/v1/maintenance"
  val getRequest = FakeRequest(GET, path)
  val jsTrue = JsBoolean(true)
  val putRequest = FakeRequest(PUT, path).withBody(jsTrue)

  "Reading the maintenance status" must behave like enabledWhenUnderMaintenance(getRequest)

  it must behave like rejectingUnauthenticatedRequests(getRequest)

  it must "return the current state when under maintenance" in new WithSampleSessions {
    services.maintenanceStatus.enterMaintenance()
    val response = regUserInGroup.doRequest(path)
    status(response) must be (OK)
    contentAsJson(response) must equal (jsTrue)
  }

  it must "return the current state when not under maintenance" in new WithSampleSessions {
    services.maintenanceStatus must not be 'underMaintenance
    val response = regUserInGroup.doRequest(path)
    status(response) must be (OK)
    contentAsJson(response) must equal (JsBoolean(false))
  }

  "Changing the maintenance status" must behave like enabledWhenUnderMaintenance(putRequest)

  it must behave like rejectingUnauthenticatedRequests(putRequest)

  it must "require operator capability" in new WithSampleSessions {
    status(regUserInGroup.submitJson(path, jsTrue, PUT)) must equal (FORBIDDEN)
  }

  it must "succeed for operator users" in new WithSampleSessions {
    services.maintenanceStatus must not be 'underMaintenance
    val response = opUser.submitJson(path, jsTrue, PUT)
    status(response) must be (OK)
    contentAsJson(response) must equal (jsTrue)
    services.maintenanceStatus must be ('underMaintenance)
  }
}
