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

import scala.concurrent.Future

import com.typesafe.config.{ConfigException, Config}
import dispatch.url

/**
 * Provider of OAuth 2.0 clients
 */
trait OAuthClientComponent {
  def oAuthClient(): OAuthClient
}

case class UserProfile(
    id: String,
    name: Option[String] = None,
    email: Option[String] = None) {
  def contact: String = {
    val parts = Seq(name, email.map { value => s"($value)" }).flatten
    if (parts.isEmpty) "--" else parts.mkString(" ")
  }
}

/**
 * OAuth client for authentication and user profile access.
 */
abstract class OAuthClient(config: Config) {

  /**
   * Link to the sign up page of the OAuth provider
   */
  def signUpUrl: String

  /**
   * Creates a link to the OAuth provider that asks for the require authorization scopes.
   *
   * @param redirectUri  URI that the OAuth provider uses to return the control
   * @return             The URL
   */
  def authenticateUrl(redirectUri: String): String

  /**
   * Request a token able to access the user profile.
   *
   * @param code  Authorization code
   * @return      Future access token or an OAuthException in case of error
   */
  def requestAccessToken(code: String): Future[String]

  /**
   * Request the user profile given an access token
   *
   * @param token  Access token
   * @return       Future user profile or an OAuthException in case of error
   */
  def requestUserProfile(token: String): Future[UserProfile]

  /**
   * OAuth client ID
   */
  def clientId = stringConfig("oauth.client.id")

  /**
   * OAuth client secret
   */
  def clientSecret = stringConfig("oauth.client.secret")

  protected def stringConfig(key: String) = try {
    config.getString(key)
  } catch {
    case ex: ConfigException.Missing =>
      throw new IllegalArgumentException(s"Missing required configuration key $key", ex)
  }

  protected def urlFromConfig(key: String) = url(stringConfig(key))
}
