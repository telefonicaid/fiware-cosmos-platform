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
import play.api.test.FakeRequest

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.controllers.common.BasicAuth
import es.tid.cosmos.api.profile.UserState.Enabled
import es.tid.cosmos.api.profile.Registration
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDao

class ApiCredentialsAuthenticationTest extends FlatSpec with MustMatchers {

  import Scalaz._

  trait WithInstance {
    val dao = new MockCosmosDao()
    val auth = new ApiCredentialsAuthentication(dao)

    val requestWithoutCredentials = FakeRequest("GET", "/sample/resource")
    def requestWithCredentials(apiKey: String, apiSecret: String) =
      requestWithoutCredentials.withHeaders("Authorization" -> BasicAuth(apiKey, apiSecret))

    val userProfile = dao.store.withTransaction { implicit c =>
      dao.profile.register(UserId("id"), Registration("user", "ssh-rsa XXX", "user@host"), Enabled)
    }
    val validApiKey = userProfile.apiCredentials.apiKey
    val invalidApiKey = stringOf(ApiCredentials.ApiKeyLength + 1)
    val validApiSecret = userProfile.apiCredentials.apiSecret
    val invalidApiSecret = stringOf(ApiCredentials.ApiSecretLength / 2)

    private def stringOf(size: Int) = Seq.fill(size)("X").mkString
  }

  "API credentials authentication" must "authenticate a valid request" in new WithInstance {
    val request = requestWithCredentials(
      userProfile.apiCredentials.apiKey, userProfile.apiCredentials.apiSecret)
    auth.authenticateRequest(request) must be (userProfile.success)
  }

  it must "fail if there is no 'Authorization' header" in new WithInstance {
    auth.authenticateRequest(requestWithoutCredentials) must be (MissingAuthentication.fail)
  }

  it must "fail if the auth header is malformed" in new WithInstance {
    val requestWithMalformedAuth = FakeRequest("GET", "/sample/resource").withHeaders(
      "Authorization" -> "malformed"
    )
    auth.authenticateRequest(requestWithMalformedAuth) must be (MalformedAuthHeader("malformed").fail)
  }

  it must "consider malformed header if keys have wrong lengths" in new WithInstance {
    for (invalidRequest <- Seq(
      requestWithCredentials(invalidApiKey, validApiSecret),
      requestWithCredentials(validApiKey, invalidApiSecret)
    )) {
      auth.authenticateRequest(invalidRequest) must be ('failure)
    }
  }

  it must "fail if the credentials doesn't belong to any user" in new WithInstance {
    val unknownCredentials = ApiCredentials.random()
    val invalidRequest = requestWithCredentials(unknownCredentials.apiKey, unknownCredentials.apiSecret)
    auth.authenticateRequest(invalidRequest) must be (InvalidAuthCredentials.fail)
  }
}
