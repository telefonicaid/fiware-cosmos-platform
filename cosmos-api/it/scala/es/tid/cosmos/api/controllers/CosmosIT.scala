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

import es.tid.cosmos.api.Global
import es.tid.cosmos.api.controllers.cosmos.CosmosResource

class CosmosIT extends FlatSpec with MustMatchers {
  "The Cosmos resource" must "return the version as JSON" in {
    running(FakeApplication(withGlobal = Some(Global))) {
      val resource = route(FakeRequest(GET, "/cosmos")).get
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
      contentAsString(resource) must include (CosmosResource.apiVersion)
    }
  }
}
