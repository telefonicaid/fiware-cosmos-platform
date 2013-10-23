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

package es.tid.cosmos.api.controllers

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test.FakeRequest
import play.api.test.Helpers._
import play.api.http.MimeTypes.{JSON, HTML}
import play.api.http.Writeable

import es.tid.cosmos.api.mocks.WithTestApplication

trait MaintenanceModeBehaviors { this: FlatSpec with MustMatchers =>

  def resourceDisabledWhenUnderMaintenance[T](
      request: FakeRequest[T])(implicit evidence: Writeable[T]) {
    rejectingWithMaintenanceMessage(request, name = "resource", expectedMimeType = JSON)
  }

  def pageDisabledWhenUnderMaintenance[T](
      request: FakeRequest[T])(implicit evidence: Writeable[T]) {
    rejectingWithMaintenanceMessage(request, name = "page", expectedMimeType = HTML)
  }

  private def rejectingWithMaintenanceMessage[T](
      request: FakeRequest[T],
      name: String,
      expectedMimeType: String)(implicit evidence: Writeable[T]) {

    it must s"reject request with 503 status as a $name under maintenance status" in
      new WithTestApplication {
        services.maintenanceStatus.enterMaintenance()
        val response = route(request).get
        status(response) must equal (SERVICE_UNAVAILABLE)
        contentType(response) must be (Some(expectedMimeType))
        contentAsString(response) must include("Service temporarily in maintenance mode")
      }

    it must s"accept request when not a $name under maintenance" in new WithTestApplication {
      status(route(request).get) must not(equal(SERVICE_UNAVAILABLE))
    }
  }
}
