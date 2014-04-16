/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
