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

package es.tid.cosmos.api.auth.oauth2

import java.util.concurrent.ExecutionException
import scala.collection.JavaConversions
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._
import scala.language.postfixOps

import com.typesafe.config.ConfigFactory
import org.scalatest.{BeforeAndAfterAll, BeforeAndAfter, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.mocks.oauth2.MockedGitHubApi
import es.tid.cosmos.api.profile.UserId
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers

class GitHubOAuthProviderTest extends FlatSpec
  with MustMatchers
  with BeforeAndAfter
  with BeforeAndAfterAll
  with FutureMatchers {

  val testTimeout = 3 seconds
  val clientId = "client-id-1"
  val clientSecret = "client-s3cr3t"
  val gitHubUser = GitHubProfile(
    id = 53,
    login = "jsmith",
    name = "John Smith",
    email = "jsmith@tid.es"
  )
  val gitHubMock = new MockedGitHubApi(
    port = 2348,
    clientId = clientId,
    clientSecret = clientSecret,
    existingUser = gitHubUser
  )
  var config = gitHubMock.configurationKeys
  var client: GitHubOAuthProvider = null

  override def beforeAll() {
    gitHubMock.start()
  }

  before {
    client = new GitHubOAuthProvider("github", toConfig(config))
  }

  after {
    gitHubMock.clear()
  }

  override def afterAll() {
    gitHubMock.stop()
  }

  private def toConfig(keys: Map[String, String]) =
    ConfigFactory.parseMap(JavaConversions.mapAsJavaMap(keys))

  "A GitHub OAuth client" must "link to signup url" in {
    client.newAccountUrl.get must be (config("signup.url"))
  }

  it must "link to an authentication url" in {
    client.authenticationUrl("http://callback") must be (
      s"${config("auth.url")}authorize?client_id=client-id-1&scope=user&redirect_uri=" +
      "http://callback")
  }

  it must "successfully request an access token with a valid code" in {
    val authUrl = client.authenticationUrl("http://callback")
    val code = gitHubMock.requestAuthorizationCode(authUrl, gitHubUser.id)
    val token_> = client.requestAccessToken(code)
    token_> must runUnder(testTimeout)
    token_> must eventually(fullyMatch regex ".{8}")
  }

  it must "handle an OAuth error when requesting an access tokens with invalid code" in {
    val token_> = client.requestAccessToken("invalid_code")
    token_> must runUnder(testTimeout)
    token_> must eventuallyFailWith [ExecutionException]
  }

  it must "request the user profile" in {
    val authUrl = client.authenticationUrl("http://callback")
    val code = gitHubMock.requestAuthorizationCode(authUrl, gitHubUser.id)
    val profile_> = for {
      token <- client.requestAccessToken(code)
      profile <- client.requestUserProfile(token)
    } yield profile
    profile_> must runUnder(testTimeout)
    profile_> must eventually(equal(OAuthUserProfile(
      id = UserId("github", "53"),
      name = Some("John Smith"),
      email = Some("jsmith@tid.es")
    )))
  }
}
