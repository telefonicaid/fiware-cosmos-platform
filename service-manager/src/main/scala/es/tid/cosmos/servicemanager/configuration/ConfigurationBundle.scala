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

/**
 * Container of different types of cluster configuration.
 *
 * @param global the global configuration. Optional and can be partial
 * @param core the core configuration. Optional and can be partial
 * @param services configuration for services. Should be complete configuration for each service
 */
case class ConfigurationBundle(
    global: Option[GlobalConfiguration],
    core: Option[CoreConfiguration],
    services: List[ServiceConfiguration]) {
  /**
   * Get the bundle's concrete configurations, i.e. excluding the optional ones without value.
   *
   * @return the bundle's concrete configurations
   */
  def configurations: List[Configuration] =  List(global, core).flatten ++ services
}

object ConfigurationBundle {
  def apply(global: GlobalConfiguration, services: ServiceConfiguration*): ConfigurationBundle =
    ConfigurationBundle(Some(global), None, services.toList)

  def apply(core: CoreConfiguration, services: ServiceConfiguration*): ConfigurationBundle =
    ConfigurationBundle(None, Some(core), services.toList)

  def apply(
      global: GlobalConfiguration,
      core: CoreConfiguration,
      services: ServiceConfiguration*): ConfigurationBundle =
    ConfigurationBundle(Some(global), Some(core), services.toList)

  def apply(services: ServiceConfiguration*): ConfigurationBundle =
    ConfigurationBundle(None, None, services.toList)

  val NoConfiguration = ConfigurationBundle(None, None, Nil)
}
