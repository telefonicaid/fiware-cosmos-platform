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

import java.net.URL

import com.typesafe.config.{Config, ConfigException}

class InfinityConfig(config: Config) {

  import InfinityConfig._

  def contentServerUrl(hostname: String): Option[URL] = {
    val protocol = mapOpt(config.getString(s"contentServer.$hostname.protocol"))
      .getOrElse(DefaultProtocol)
    val port = mapOpt(config.getInt(s"contentServer.$hostname.port"))
    val basePath = mapOpt(config.getString(s"contentServer.$hostname.basePath"))
      .getOrElse(DefaultBasePath)
    port.map(new URL(protocol, hostname, _, basePath))
  }

  private def mapOpt[T](f: => T): Option[T] = try { Some(f) } catch {
    case _: ConfigException.Missing => None
  }
}

object InfinityConfig {

  val DefaultProtocol = "https"
  val DefaultBasePath = "/infinityfs/v1"
}
