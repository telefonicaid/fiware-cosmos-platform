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

package es.tid.cosmos.servicemanager.ambari.configuration

import java.io.File
import scala.collection.JavaConversions._

import com.typesafe.config._

/** Utility to inject custom properties into Typesafe configurations.
  *
  * If you parse a configuration with this add-on and have the line
  * {{{include "cluster-properties}}} in it, interpolations such as {{{${MasterNode}}}
  * will be replaced as expected.
  */
private class PropertiesInterpolation (
    properties: ConfigProperties, fallback: Option[ConfigIncluder]) extends ConfigIncluder {

  def this(properties: ConfigProperties) = this(properties, None)

  override def withFallback(fallback: ConfigIncluder): ConfigIncluder =
    new PropertiesInterpolation(properties, Option(fallback))

  override def include(context: ConfigIncludeContext, what: String): ConfigObject = what match {
    case "cluster-properties" =>
      ConfigValueFactory.fromMap(properties.map(tuple => (tuple._1.toString, tuple._2)))
    case _ =>
      fallback.fold(PropertiesInterpolation.EmptyConfigObject)(_.include(context, what))
  }
}

private[configuration] object PropertiesInterpolation {

  /** Parses a configuration file with custom properties interpolation */
  def parseWithProperties(file: File, properties: ConfigProperties): Config = {
    val interpolation = new PropertiesInterpolation(properties)
    val parseOptions = ConfigParseOptions.defaults().setIncluder(interpolation)
    ConfigFactory.parseFileAnySyntax(file, parseOptions).resolve()
  }

  private val EmptyConfigObject: ConfigObject = ConfigValueFactory.fromMap(Map[String, String]())
}
