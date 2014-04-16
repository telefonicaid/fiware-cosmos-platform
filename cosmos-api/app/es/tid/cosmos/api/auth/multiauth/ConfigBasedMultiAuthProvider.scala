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

package es.tid.cosmos.api.auth.multiauth

import scala.collection.JavaConversions._
import scala.util.control.NonFatal

import com.typesafe.config.{ConfigException, Config}

import es.tid.cosmos.api.auth.AuthProvider
import es.tid.cosmos.api.auth.oauth2.OAuthProvider

private[multiauth] class ConfigBasedMultiAuthProvider(config: Config)
  extends MultiAuthProvider {

  private val authConfig = try {
    config.getConfig("auth")
  } catch {
    case ex: ConfigException.Missing =>
      throw new IllegalArgumentException("No authentication provider was defined", ex)
  }

  override val providers: Map[String, AuthProvider] = (for {
    name <- enabledProviderNames
    provider = instantiateProvider(name, providerConfig(name))
  } yield (name, provider)).toMap

  override val tokenAuthenticationProvider: Option[OAuthProvider] = {
    if (isTokenAuthenticationEnabled) {
      val name = try {
        config.getString("tokenAuth.provider")
      } catch {
        case ex: ConfigException.Missing => throw new IllegalArgumentException(
          "Token authentication was enabled but no provider was defined")
      }
      Some(oauthProviders.get(name)
        .getOrElse(throw new IllegalArgumentException(
          s"Cannot use '$name' since it is not an enabled, OAuth provider")))
    } else None
  }

  private def isTokenAuthenticationEnabled = try {
    config.getBoolean("tokenAuth.enabled")
  } catch {
    case ex: ConfigException.Missing => false
  }

  private lazy val enabledProviderNames = {
    val providers = authConfig.root().keySet().toSet
    val enabledProviders = providers.filter(isEnabled)
    require(!enabledProviders.isEmpty,
      s"No authentication provider was enabled. Defined providers: ${providers.mkString(", ")}.")
    enabledProviders
  }

  private def isEnabled(name: String) = try {
    providerConfig(name).getBoolean("enabled")
  } catch {
    case _: ConfigException.Missing => false
  }

  private def instantiateProvider(name: String, providerConf: Config): AuthProvider = try {
    val clazz = Class.forName(providerConf.getString("class"))
      .asInstanceOf[Class[_ <: AuthProvider]]
    val constructor = clazz.getConstructor(classOf[String], classOf[Config])
    constructor.newInstance(name, providerConf)
  } catch {
    case NonFatal(ex) => throw new IllegalArgumentException(
      s"Cannot initialize the '$name' authentication provider, " +
        s"check that its configuration ('auth.$name.*') is correct", ex)
  }

  private def providerConfig(name: String) = authConfig.getConfig(name)
}
