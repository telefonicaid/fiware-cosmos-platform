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

package es.tid.cosmos.api.auth.oauth2.keyrock

import java.util.concurrent.ExecutionException
import scala.collection.JavaConversions
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._
import scala.language.postfixOps

import com.typesafe.config.ConfigFactory
import org.scalatest.{BeforeAndAfterAll, BeforeAndAfter, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.profile.UserId
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers

class ProviderIT extends FlatSpec
  with MustMatchers
  with BeforeAndAfter
  with BeforeAndAfterAll
  with FutureMatchers {

  val testTimeout = 3 seconds
  val clientId = "client-id-1"
  val clientSecret = "client-s3cr3t"
  val redirectUri = "http://callback"
  val userProfile = KeyrockProfile(
    id = 53,
    actorId = 112,
    nickName = "jackie",
    displayName = "John Smith",
    email = "jsmith@tid.es"
  )
  val serverMock = new MockedKeyrockApi(
    port = 2349,
    clientId = clientId,
    clientSecret = clientSecret,
    existingUser = userProfile,
    redirectUri = redirectUri
  )
  var config = serverMock.configurationKeys
  var client: Provider = null

  override def beforeAll() {
    serverMock.start()
  }

  before {
    client = new Provider("keyrock", toConfig(config))
  }

  after {
    serverMock.clear()
  }

  override def afterAll() {
    serverMock.stop()
  }

  private def toConfig(keys: Map[String, String]) =
    ConfigFactory.parseMap(JavaConversions.mapAsJavaMap(keys))

  "A Keyrock OAuth client" must "link to signup url" in {
    client.newAccountUrl.get must be (config("signup.url"))
  }

  it must "link to an authentication url" in {
    client.authenticationUrl(redirectUri) must be (s"${config("auth.url")}authorize?" + Seq(
      "response_type=code",
      s"client_id=$clientId",
      s"redirect_uri=$redirectUri"
    ).mkString("&"))
  }

  it must "successfully request an access token with a valid code" in {
    val authUrl = client.authenticationUrl(redirectUri)
    val code = serverMock.requestAuthorizationCode(authUrl, userProfile.id)
    val token_> = client.requestAccessToken(code, redirectUri)
    token_> must (runUnder(testTimeout) and eventually(have length 8))
  }

  it must "handle an OAuth error when requesting an access tokens with invalid code" in {
    val token_> = client.requestAccessToken("invalid_code", redirectUri)
    token_> must runUnder(testTimeout)
    token_> must eventuallyFailWith [ExecutionException]
  }

  it must "request the user profile" in {
    val authUrl = client.authenticationUrl(redirectUri)
    val code = serverMock.requestAuthorizationCode(authUrl, userProfile.id)
    val profile_> = for {
      token <- client.requestAccessToken(code, redirectUri)
      profile <- client.requestUserProfile(token)
    } yield profile
    profile_> must runUnder(testTimeout)
    profile_> must eventually(equal(OAuthUserProfile(
      id = UserId("keyrock", "112"),
      name = Some("John Smith"),
      email = Some("jsmith@tid.es")
    )))
  }
}
