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

package es.tid.cosmos.api.auth

import scala.collection.JavaConversions._
import scala.util.control.NonFatal

import com.typesafe.config.{ConfigException, Config}

import es.tid.cosmos.common.ConfigComponent

private[auth] class ConfigBasedMultiAuthProvider(config: Config)
  extends MultiAuthProvider {

  private val authConfig = try {
    config.getConfig("auth")
  } catch {
    case ex: ConfigException.Missing =>
      throw new IllegalArgumentException("No authentication provider was defined", ex)
  }

  override val providers: Map[String, AuthProvider] = (for {
    name <- findEnabledProviders
    provider = instantiateProvider(name, providerConfig(name))
  } yield (name, provider)).toMap

  private def findEnabledProviders = {
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

trait ConfigBasedMultiAuthProviderComponent extends MultiAuthProviderComponent {
  this: ConfigComponent =>

  override val multiAuthProvider: MultiAuthProvider =
    new ConfigBasedMultiAuthProvider(config)
}
