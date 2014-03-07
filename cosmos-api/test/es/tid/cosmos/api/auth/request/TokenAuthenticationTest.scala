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

import scala.concurrent.Future
import scalaz._

import org.mockito.BDDMockito.given
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import play.api.test.FakeRequest

import es.tid.cosmos.api.auth.oauth2.{OAuthUserProfile, OAuthProvider}
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.UserState.Enabled
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDao

class TokenAuthenticationTest extends FlatSpec with MustMatchers with MockitoSugar {

  import Scalaz._

  trait WithInstance {
    val oauth = mock[OAuthProvider]
    val dao = new MockCosmosDao()
    val token = "oauth-token"
    val oauthProfile = OAuthUserProfile(UserId("id"))
    val requestWithoutToken = FakeRequest("GET", "/resource")
    val requestWithToken = requestWithoutToken.withHeaders("X-Auth-Token" -> token)
    val authentication = new TokenAuthentication(oauth, dao)

    def givenTokenIsValid() {
      given(oauth.requestUserProfile(token)).willReturn(Future.successful(oauthProfile))
    }

    def givenTokenIsInvalid() {
      given(oauth.requestUserProfile(token)).willReturn(Future.failed(new Error("Invalid token")))
    }

    def registerUser() = dao.store.withTransaction { implicit c =>
      dao.profile.register(
        oauthProfile.id, Registration("handle", "ssh-rsa XXX", "user@host"), Enabled)
    }
  }

  "Token based authentication" must "authenticate valid user tokens" in new WithInstance {
    val cosmosProfile = registerUser()
    givenTokenIsValid()
    authentication.authenticateRequest(requestWithToken) must be (cosmosProfile.success)
  }

  it must "reject OAuth users without Cosmos profile" in new WithInstance {
    givenTokenIsValid()
    authentication.authenticateRequest(requestWithToken) must be (InvalidAuthCredentials.failure)
  }

  it must "reject requests with invalid tokens" in new WithInstance {
    registerUser()
    givenTokenIsInvalid()
    val result = authentication.authenticateRequest(requestWithToken)
    result must be ('failure)
    result.toString must include ("Cannot retrieve user profile")
  }

  it must "reject request without token header" in new WithInstance {
    authentication.authenticateRequest(requestWithoutToken) must be (MissingAuthentication.failure)
  }
}
