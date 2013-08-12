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

package es.tid.cosmos.api.mocks.oauth2

import scala.concurrent.Future

import es.tid.cosmos.api.oauth2._

class MockOAuthClient extends OAuthClient {
  import MockOAuthClient._

  /**
   * Link to the sign up page of the OAuth provider
   */
  def signUpUrl: String = s"$BaseUrl/signup/"

  /**
   * Creates a link to the OAuth provider that asks for the require authorization scopes.
   *
   * @param redirectUri  URI that the OAuth provider uses to return the control
   * @return             The URL
   */
  def authenticateUrl(redirectUri: String): String =
    s"$BaseUrl/oauth?client_id=fake&redirect_to=$redirectUri"

  /**
   * Request a token able to access the user profile. Only works with grantedCode.
   *
   * @param code  Authorization code
   * @return      Future access token or an OAuthException in case of error
   */
  def requestAccessToken(code: String): Future[String] =
    if (code == GrantedCode) Future.successful(GrantedToken)
    else Future.failed(OAuthException(OAuthError.InvalidGrant, "testing invalid grant"))

  /**
   * Request the user profile given an access token. Only works with grantedToken.
   *
   * @param token  Access token
   * @return       Future user profile or an OAuthException in case of error
   */
  def requestUserProfile(token: String): Future[UserProfile] =
    if (token == GrantedToken) Future.successful(User)
    else Future.failed(OAuthException(OAuthError.InvalidRequest, "testing invalid requests"))
}

object MockOAuthClient {
  val BaseUrl = "http://mock-oauth"
  val GrantedCode = "fake-code"
  val GrantedToken = "fake-token-123"
  val User = UserProfile("101", Some("John Smith"), Some("jsmith@tid.es"))
}

trait MockOAuthClientComponent extends OAuthClientComponent {
  def oAuthClient(): OAuthClient = new MockOAuthClient()
}
