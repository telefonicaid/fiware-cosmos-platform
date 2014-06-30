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

import es.tid.cosmos.servicemanager.configuration._

/** Read configuration contributions from file.
  *
  * The loader will load the configuration found in the classpath `[configName].conf` file
  * as specified by `configName`.
  *
  * The Loader makes use of TypeSafe's Config class, so any syntax supported by that class is
  * supported by the FileConfigurationContributor.
  *
  * @see [[https://github.com/typesafehub/config About TypeSafe's config]]
  * @see [[https://github.com/typesafehub/config/blob/master/HOCON.md About HOCON format]]
  */
class FileConfigurationContributor(
    configPath: String,
    configName: String,
    associatedService: Boolean = false,
    extraProperties: ConfigProperties = Map.empty)
  extends ConfigurationContributor {

  private val bundleParser = new ConfigurationBundleParser(associatedService)

  /** Builder of service description configurations.
    *
    * For the service `configName` it loads its configuration from a file and it looks for 3 types of
    * configuration: Global, Core and Service. They are all optional.
    *
    * The Service configuration is a list of configurations.
    *
    * In order for the configuration file to be able to use dynamic properties, the configuration
    * file must include a `include "cluster-properties` statement.
    *
    * configuration example:
    * {{{
    *
    * include "cluster-properties"
    *
    *"global" {
    *  "properties" {
    *    "global.example.string"="global-"${MasterNode}
    *   }
    *}
    *
    *"core" {
    *  "properties" {
    *    "core.example"="core-"${MasterNode}
    *   }
    *}
    *
    *"services" = [
    *  {
    *    "configType"="test-service-site1"
    *    "tag"="aTag",
    *    "properties" {
    *      "service.example1"="service-"${MasterNode}
    *     }
    *  },
    *
    *  {
    *    "configType"="test-service-site2"
    *    "tag"="aTag",
    *    "properties" {
    *      "service.example2"="service-"${MasterNode}
    *     }
    *  }
    *]
    *}}}
    *
    * @param properties the dynamic properties to be injected to the configuration contributions
    * @see [[ConfigurationKeys]]
    */
  override def contributions(properties: ConfigProperties): ConfigurationBundle =
    bundleParser.parse(resolveConfig(properties ++ extraProperties))

  private def resolveConfig(properties: ConfigProperties) =
    PropertiesInterpolation.parseWithProperties(new File(s"$configPath/$configName"), properties)
}

