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

import scala.util.control.NonFatal

import com.typesafe.config.{Config, ConfigException}

case class ServiceConfig(
  name: String,
  infinityHostname: String,
  infinityPort: Int
)

object ServiceConfig {

  def infinityHostnameProperty(serviceName: String) =
    s"infinity.server.services.$serviceName.infinity.hostname"

  def infinityPortProperty(serviceName: String) =
    s"infinity.server.services.$serviceName.infinity.port"

  def apply(name: String, config: Config): Option[ServiceConfig] = try {
    Some(ServiceConfig(
      name = name,
      infinityHostname = config.getString(infinityHostnameProperty(name)),
      infinityPort = config.getInt(infinityPortProperty(name))
    ))
  } catch {
    case e: ConfigException.Missing => None
    case NonFatal(e) => throw new IllegalStateException(s"cannot service config for '$name'", e)
  }

  def active(config: Config): ServiceConfig = try {
    val activeServiceProperty = "infinity.server.services.active"
    ServiceConfig(config.getString(activeServiceProperty), config).getOrElse(
      throw new IllegalArgumentException(
        s"missing $activeServiceProperty property in application config"))

  } catch {
    case e: ConfigException =>
      throw new IllegalStateException("cannot obtain active service config", e)
  }
}
