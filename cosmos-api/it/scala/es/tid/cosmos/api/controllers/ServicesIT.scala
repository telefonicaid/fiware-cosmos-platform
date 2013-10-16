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
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.mocks.WithTestApplication

class ServicesIT extends FlatSpec with MustMatchers {
  "Services listing" must "list services provided by Service Manager" in new WithTestApplication() {
    val resource = route(FakeRequest(GET, "/cosmos/v1/services")).get
    status(resource) must equal (OK)
    contentType(resource) must be (Some("application/json"))
    contentAsString(resource) must include ("HDFS")
  }
}
