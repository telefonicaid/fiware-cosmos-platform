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

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.ambari.configuration._

/** Mix-in for ambari services that populates the configuration from a file named as the
  * lowercased name of the service.
  */
trait FileConfiguration { this: AmbariService =>

  def extraProperties(parametrization: service.Parametrization): ConfigProperties = Map.empty

  override def configurator(parametrization: service.Parametrization, configPath: String) =
    new FileConfigurationContributor(
      configPath, service.name.toLowerCase, Some(service), extraProperties(parametrization))
}
