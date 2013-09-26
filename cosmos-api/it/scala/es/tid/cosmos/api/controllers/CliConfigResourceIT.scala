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
import play.api.mvc.Session
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.mocks.WithTestApplication
import es.tid.cosmos.api.mocks.oauth2.MockOAuthConstants

class CliConfigResourceIT extends FlatSpec with MustMatchers {

  "A CLI config resource" must "produce a 404 status when not authenticated and registered" in
    new WithTestApplication {
      status(route(FakeRequest(GET, "/cosmosrc")).get) must be (NOT_FOUND)
    }

  it must "return a cosmosrc when authenticated" in new WithTestApplication {
    val cosmosId = registerUser(dao, MockOAuthConstants.User101)
    val profile = dao.withConnection { implicit c =>
      dao.lookupByUserId(MockOAuthConstants.User101.id).get
    }
    val session = new Session()
          .setUserProfile(MockOAuthConstants.User101)
          .setToken("token")
          .setCosmosId(cosmosId)
    val result = route(withSession(FakeRequest(GET, "/cosmosrc"), session)).get
    status(result) must be (OK)
    contentAsString(result) must (include (s"api_key: ${profile.apiCredentials.apiKey}") and
      include (s"api_secret: ${profile.apiCredentials.apiSecret}"))
  }
}
