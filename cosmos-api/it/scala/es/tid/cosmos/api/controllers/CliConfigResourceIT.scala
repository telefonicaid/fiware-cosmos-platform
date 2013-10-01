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
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.ResultMatchers.redirectTo
import es.tid.cosmos.api.controllers.pages.WithSampleSessions

class CliConfigResourceIT extends FlatSpec with MustMatchers {

  "A CLI config resource" must "redirect when not authenticated and registered" in
    new WithSampleSessions {
      unauthUser.doRequest("/cosmosrc") must redirectTo ("/")
      unregUser.doRequest("/cosmosrc") must redirectTo ("/register")
    }

  it must "return a cosmosrc when authenticated" in new WithSampleSessions {
    val response = regUser.doRequest("/cosmosrc")
    status(response) must be (OK)
    contentAsString(response) must (
      include (s"api_key: ${regUser.cosmosProfile.apiCredentials.apiKey}") and
      include (s"api_secret: ${regUser.cosmosProfile.apiCredentials.apiSecret}"))
  }
}
