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

import com.typesafe.config.ConfigException

import es.tid.cosmos.platform.common.ConfigComponent

trait ConfigurableOAuthClientComponent extends OAuthClientComponent {
  this: ConfigComponent =>

  private val configKey = "oauth.provider"

  private lazy val factories: Map[String, () => OAuthClient] = Map(
    "tuid" -> (() => new TuIdOAuthClient(config)),
    "github" -> (() => new GitHubOAuthClient(config))
  )

  lazy val oAuthClient: OAuthClient = {
    val providerName = try {
      config.getString(configKey).toLowerCase
    } catch {
      case ex: ConfigException.Missing => throw new ExceptionInInitializerError(
        s"Missing OAuth provider configuration. Add $configKey to the configuration file.")
    }
    factories.get(providerName)
      .map(factory => factory())
      .getOrElse(throw new ExceptionInInitializerError(s"Unknown OAuth provider '$providerName'"))
  }
}
