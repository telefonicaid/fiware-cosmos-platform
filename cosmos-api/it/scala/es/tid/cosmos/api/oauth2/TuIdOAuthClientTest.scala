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

package es.tid.cosmos.api.oauth2

import java.util.concurrent.ExecutionException
import scala.concurrent.duration._
import scala.concurrent.ExecutionContext.Implicits.global

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.mocks.WithTestApplication
import es.tid.cosmos.api.mocks.oauth2.{User, WithMockedTuIdService}
import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers

class TuIdOAuthClientTest extends FlatSpec with MustMatchers with FutureMatchers {

  val TestTimeout = 3 seconds

  "A TU|id client" must "link to signup url" in new WithTestApplication {
    TuIdOAuthClient.signUpUrl must be ("http://localhost:8763/signup/validate/")
  }

  it must "link to an authentication url" in new WithTestApplication {
    TuIdOAuthClient.authenticateUrl("http://callback") must be (
      "http://localhost:8763/authorize?client_id=fake client id&" +
      "scope=userdata.user.read.basic userdata.user.read.emails&" +
      "response_type=code&redirect_uri=http://callback")
  }

  it must "successfully request an access token with a valid code" in new WithMockedTuIdService {
    val authUrl = TuIdOAuthClient.authenticateUrl("http://callback")
    val code = tuId.requestAuthorizationCode(authUrl, tuId.users(0).id)
    val token_> = TuIdOAuthClient.requestAccessToken(code)
    token_> must runUnder(TestTimeout)
    token_> must eventually(startWith("access-"))
  }

  it must "handle an OAuth error when requesting an access tokens with invalid code" in
    new WithMockedTuIdService {
      val token_> = TuIdOAuthClient.requestAccessToken("invalid_code")
      token_> must runUnder(TestTimeout)
      token_> must eventuallyFailWith [ExecutionException]
    }

  it must "request the user profile" in new WithMockedTuIdService {
    val user = tuId.users(0)
    val authUrl = TuIdOAuthClient.authenticateUrl("http://callback")
    val code = tuId.requestAuthorizationCode(authUrl, user.id)
    val profile_> = for {
      token <- TuIdOAuthClient.requestAccessToken(code)
      profile <- TuIdOAuthClient.requestUserProfile(token)
    } yield profile
    profile_> must runUnder(TestTimeout)
    profile_> must eventually(equal(user))
  }
}
