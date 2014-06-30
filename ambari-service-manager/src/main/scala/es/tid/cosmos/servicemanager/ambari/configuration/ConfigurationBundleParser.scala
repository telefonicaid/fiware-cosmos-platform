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

import scala.collection.JavaConversions._

import com.typesafe.config.Config

import es.tid.cosmos.servicemanager.configuration._

/** Parses configuration contributions from a Typesafe config object.
  *
  * @constructor
  * @param associatedService   Associated service, "services" section will be rejected when None
  *
  * @see [[FileConfigurationContributor]]
  */
private[configuration] class ConfigurationBundleParser(associatedService: Boolean) {

  /** Parse a Config into a ConfigurationBundle
    *
    * @param config  Config to parse
    * @return        Equivalent ConfigurationBundle
    * @throws IllegalArgumentException if the input is not valid
    */
  def parse(config: Config): ConfigurationBundle = ConfigurationBundle(
    systemConfiguration("global", GlobalConfiguration, config),
    systemConfiguration("core", CoreConfiguration, config),
    serviceConfigurations(config)
  )

  /** Parses a core or global configuration given its name and constructor */
  private def systemConfiguration[T](
     name: String, constructor: Map[String, Any] => T, config: Config): Option[T] = {
    val key = s"$name.properties"
    if (config.hasPath(key)) Some(constructor(config.getObject(key).unwrapped().toMap))
    else None
  }

  private def serviceConfigurations(config: Config): List[ServiceConfiguration] =
    if (!config.hasPath("services")) List.empty
    else {
      require(associatedService, "Cannot contribute to a service if no associated service exists")
      parseServicesConfigurations(config)
    }

  private def parseServicesConfigurations(config: Config): List[ServiceConfiguration] =
    (for (innerConfig <- config.getConfigList("services");
          configType = innerConfig.getString("configType");
          serviceProperties = innerConfig.getObject("properties").unwrapped().toMap)
      yield ServiceConfiguration(configType, serviceProperties)).toList
}
