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

package es.tid.cosmos.servicemanager.configuration

/** Abstract representation of a cluster configuration. */
trait Configuration extends ConfigurationHeader {
  def properties: Map[String, Any]
}

/** Representation of a cluster's global configuration. Can be partial.
  *
  * @param properties the global configuration properties
  */
case class GlobalConfiguration(properties: Map[String, Any]) extends Configuration {
  final val configType: String = "global"
}

/** Representation of a cluster's core configuration. Can be partial.
  *
  * @param properties the core configuration properties
  */
case class CoreConfiguration(properties: Map[String, Any]) extends Configuration {
  final val configType: String = "core-site"
}

/** Representation of a service <b>complete</b> configuration.
  *
  * @param configType the type for this service configuration
  * @param properties the service configuration properties
  */
case class ServiceConfiguration(final val configType: String, properties: Map[String, Any])
  extends Configuration {

  /** Merge with another service configuration as long as it has the same type and
    * there are no conflicting keys.
    *
    * @param other  Configuration to merge with
    * @return       Merged config
    */
  @throws[ConfigurationConflict]("If keys conflict")
  def merge(other: ServiceConfiguration): ServiceConfiguration = {
    require(other.configType == configType)
    val conflictingKeys = properties.keySet intersect other.properties.keySet
    if (!conflictingKeys.isEmpty) {
      throw new ConfigurationConflict("Conflicting keys in service configuration " +
        s"for $configType: ${conflictingKeys.mkString(",")}")
    }
    copy(properties = properties ++ other.properties)
  }
}
