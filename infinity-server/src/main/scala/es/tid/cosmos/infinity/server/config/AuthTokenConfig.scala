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

package es.tid.cosmos.infinity.server.config

import java.util.concurrent.TimeUnit

import com.typesafe.config.{Config, ConfigException}

/** The auth token configuration of Infinity Server. */
case class AuthTokenConfig(
  secret: String,
  pathTemplate: String,
  phraseTemplate: String,
  duration: Long
)

object AuthTokenConfig extends ConfigUtils {

  val SecretProperty = "infinity.server.auth-token.secret"
  val PathTemplateProperty = "infinity.server.auth-token.path"
  val PhraseTemplateProperty = "infinity.server.auth-token.phrase"
  val DurationProperty = "infinity.server.auth-token.duration"

  val DefaultPathTemplate = "/p/${token}/${expire}${path}"
  val DefaultPhraseTemplate = "${secret}${expire}${path}${query}"
  val DefaultDuration = 60

  /** Retrieve the auth token configuration from the actor system config. */
  def fromConfig(config: Config): AuthTokenConfig = try {
    new AuthTokenConfig(
      secret = config.getString(SecretProperty),
      pathTemplate = config.getStringOpt(PathTemplateProperty).getOrElse(DefaultPathTemplate),
      phraseTemplate = config.getStringOpt(PhraseTemplateProperty).getOrElse(DefaultPhraseTemplate),
      duration = config.getDurationOpt(DurationProperty, TimeUnit.SECONDS).getOrElse(DefaultDuration)
    )
  } catch {
    case e: ConfigException => throw new IllegalStateException(
      s"invalid $SecretProperty property in auth token config", e)
  }
}
