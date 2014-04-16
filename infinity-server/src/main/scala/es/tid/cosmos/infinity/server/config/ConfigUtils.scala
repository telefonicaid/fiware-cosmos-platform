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

import com.typesafe.config.{ConfigException, Config}

trait ConfigUtils {

  implicit class OptionalConfig(config: Config) {

    /** Retrieve an optional string from the given config path.
      *
      * @param path the config path whose value is to be retrieved
      * @return Some(_) with the config value, or None if path is missing
      */
    def getStringOpt(path: String): Option[String] = getOpt {
      config.getString(path)
    }

    /** Retrieve an optional duration from the given config path.
      *
      * @param path the config path whose value is to be retrieved
      * @return Some(_) with the config value, or None if path is missing
      */
    def getDurationOpt(path: String, units: TimeUnit): Option[Long] = getOpt {
      config.getDuration(path, units)
    }

    private def getOpt[T](value: => T): Option[T] = try {
      Some(value)
    } catch {
      case e: ConfigException => None
    }
  }
}
