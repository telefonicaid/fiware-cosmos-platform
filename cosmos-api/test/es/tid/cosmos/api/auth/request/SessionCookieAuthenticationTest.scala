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

package es.tid.cosmos.api.auth.request

import scalaz._

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.mvc.Session
import play.api.test.{FakeHeaders, FakeRequest}

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.profile._

class SessionCookieAuthenticationTest extends FlatSpec with MustMatchers {

  import Scalaz._

  trait WithInstance {
    val dao = new MockCosmosProfileDao()
    val userId = UserId("id")
    val userProfile = dao.withTransaction { implicit c =>
      dao.registerUser(userId, Registration("user", "ssh-rsa XXXX", "user@host"), UserState.Enabled)
    }
    val auth = new SessionCookieAuthentication(dao)

    val requestWithoutSession = FakeRequest("GET", "/sample/path")

    def requestWithSession(userId: UserId) = new FakeRequest(
      "GET", "/sample/path", headers = FakeHeaders(), body = "") {
      override lazy val session: Session = new Session()
        .setUserProfile(OAuthUserProfile(userId))
        .setToken("Token")
    }
  }

  "Session cookie authentication" must "succeed with the user profile" in new WithInstance {
    auth.authenticateRequest(requestWithSession(userId)) must be (userProfile.success)
  }

  it must "fail with missing auth error when there is no session" in
    new WithInstance {
      auth.authenticateRequest(requestWithoutSession) must be (MissingAuthentication.fail)
    }

  it must "fail with missing auth error when session correspond to an unknown user" in
    new WithInstance {
      val request = requestWithSession(UserId("unknown"))
      auth.authenticateRequest(request) must be (MissingAuthentication.fail)
    }
}
