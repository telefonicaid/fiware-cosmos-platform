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
