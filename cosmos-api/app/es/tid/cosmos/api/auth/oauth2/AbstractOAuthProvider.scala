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

package es.tid.cosmos.api.auth.oauth2

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future

import com.typesafe.config.{ConfigException, Config}
import dispatch.url

/** OAuth client for authentication and user profile access. */
private[oauth2] abstract class AbstractOAuthProvider(
    override val id: String,
    config: Config
  ) extends OAuthProvider {

  override val name = stringConfig("name")

  override val newAccountUrl: Option[String] = try {
    Some(config.getString("signup.url"))
  } catch {
    case _: ConfigException.Missing => None
  }

  override def requestUserProfile(token: String): Future[OAuthUserProfile] =
    requestProfileResource(token).map(profileParser.parse)

  override lazy val adminPassword: Option[String] = {
    val isEnabled = try {
      config.getBoolean("externalAdmin.enabled")
    } catch {
      case _: ConfigException.Missing => false
    }
    if (!isEnabled) None
    else try {
      Some(config.getString("externalAdmin.password"))
    } catch {
      case _: ConfigException.Missing => throw new IllegalArgumentException(
        s"auth.$id.externalAdmin.password is mandatory when externalAdmin is enabled")
    }
  }

  /** OAuth client ID */
  protected val clientId = stringConfig("client.id")

  /** OAuth client secret */
  protected val clientSecret = stringConfig("client.secret")

  /** Request the profile resource contents. */
  protected def requestProfileResource(token: String): Future[String]

  protected val profileParser: ProfileParser

  /** Get a required configuration key
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
