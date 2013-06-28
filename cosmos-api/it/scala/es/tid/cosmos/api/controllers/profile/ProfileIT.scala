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

package es.tid.cosmos.api.controllers.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.AuthBehaviors
import es.tid.cosmos.api.mocks.WithSampleUsers

class ProfileIT extends FlatSpec with MustMatchers with AuthBehaviors {

  val profileResource = "/cosmos/v1/profile"

  "The profile resource" must behave like
    rejectingUnauthenticatedRequests(FakeRequest(GET, profileResource))

  it must "return profile information of the API keys owner" in new WithSampleUsers {
    val response = route(FakeRequest(GET, profileResource).authorizedBy(user1)).get
    status(response) must be (OK)
    contentAsString(response) must include ("user1")
  }
}
