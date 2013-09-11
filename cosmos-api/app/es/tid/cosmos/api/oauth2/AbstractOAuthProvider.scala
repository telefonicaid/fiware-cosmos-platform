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

import com.typesafe.config.{ConfigException, Config}
import dispatch.url

/**
 * OAuth client for authentication and user profile access.
 */
abstract class AbstractOAuthProvider(
    override val id: String,
    config: Config
  ) extends OAuthProvider {

  override val name = stringConfig("name")

  override val newAccountUrl: Option[String] = try {
    Some(config.getString("signup.url"))
  } catch {
    case _: ConfigException.Missing => None
  }

  /**
   * OAuth client ID
   */
  protected val clientId = stringConfig("client.id")

  /**
   * OAuth client secret
   */
  protected val clientSecret = stringConfig("client.secret")

  /**
   * Get a required configuration key
   * @param key Configuration key (relative to the OAuth provider conf)
   * @return    An string
   * @throws IllegalArgumentException If the key is missing
   */
  protected def stringConfig(key: String) = try {
    config.getString(key)
  } catch {
    case ex: ConfigException.Missing =>
      throw new IllegalArgumentException(s"Missing required configuration key auth.$id.$key", ex)
  }

  protected def authorizationUrl = urlFromConfig("auth.url")

  protected def apiUrl = urlFromConfig("api.url")

  private def urlFromConfig(key: String) = url(stringConfig(key))
}
