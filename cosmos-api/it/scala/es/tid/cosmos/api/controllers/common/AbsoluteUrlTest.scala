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
import play.api.test.{FakeApplication, FakeRequest, WithApplication}

import es.tid.cosmos.api.controllers.cosmos.routes

class AbsoluteUrlTest extends FlatSpec with MustMatchers {

  implicit val request = FakeRequest()

  "Absolute URL factory" must "create HTTP URLs by default" in new WithApplication {
    AbsoluteUrl(routes.CosmosResource.version()) must startWith ("http://")
  }

  val sslApp = FakeApplication(additionalConfiguration = Map("application.ssl.enabled" -> "true"))

  it must "create HTTPS URLs when configured for it" in new WithApplication(sslApp) {
    AbsoluteUrl(routes.CosmosResource.version()) must startWith ("https://")
  }
}
