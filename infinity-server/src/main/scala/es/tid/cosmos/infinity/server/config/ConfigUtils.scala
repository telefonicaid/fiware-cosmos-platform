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
