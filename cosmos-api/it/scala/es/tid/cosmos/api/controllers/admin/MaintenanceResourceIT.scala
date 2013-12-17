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
    val response = regUser.doRequest(path)
    status(response) must be (OK)
    contentAsJson(response) must equal (jsTrue)
  }

  it must "return the current state when not under maintenance" in new WithSampleSessions {
    services.maintenanceStatus must not be 'underMaintenance
    val response = regUser.doRequest(path)
    status(response) must be (OK)
    contentAsJson(response) must equal (JsBoolean(false))
  }

  "Changing the maintenance status" must behave like enabledWhenUnderMaintenance(putRequest)

  it must behave like rejectingUnauthenticatedRequests(putRequest)

  it must "require operator capability" in new WithSampleSessions {
    status(regUser.submitJson(path, jsTrue, PUT)) must equal (FORBIDDEN)
  }

  it must "succeed for operator users" in new WithSampleSessions {
    services.maintenanceStatus must not be 'underMaintenance
    val response = opUser.submitJson(path, jsTrue, PUT)
    status(response) must be (OK)
    contentAsJson(response) must equal (jsTrue)
    services.maintenanceStatus must be ('underMaintenance)
  }
}
