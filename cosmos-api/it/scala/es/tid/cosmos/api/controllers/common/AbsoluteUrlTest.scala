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

package es.tid.cosmos.api.controllers.common

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test.FakeRequest

import es.tid.cosmos.api.controllers.cosmos.routes
import es.tid.cosmos.api.mocks.WithTestApplication

class AbsoluteUrlTest extends FlatSpec with MustMatchers {

  implicit val request = FakeRequest()

  "Absolute URL factory" must "create HTTP URLs by default" in new WithTestApplication {
    AbsoluteUrl(routes.CosmosResource.get()) must startWith ("http://")
  }

  it must "create HTTPS URLs when configured for it" in
    new WithTestApplication(Map("application.ssl.enabled" -> "true")) {
      AbsoluteUrl(routes.CosmosResource.get()) must startWith ("https://")
    }
}
