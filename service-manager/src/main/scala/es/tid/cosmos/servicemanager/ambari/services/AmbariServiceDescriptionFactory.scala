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

import es.tid.cosmos.servicemanager.{ComponentDescription, Service}
import es.tid.cosmos.servicemanager.ambari.configuration._

/** Factory object to turn service descriptions to ambari-specific ones. */
object AmbariServiceDescriptionFactory {
  /** Turn a service to an `AmbariServiceDescription`.
    * If the service is found not to be a `ConfigurationContributor` it wraps it with a
    * `ServiceWithConfigurationFile`.
    *
    * If the given service is already an `AmbariServiceDescription` it will simply return it.
    *
    * @param service                    the service to decorate
    * @param configDirectoryPath the directory path to find the service configuration file
    */
  def toAmbariService(service: Service, configDirectoryPath: String)
      : AmbariService =
    service match {
      case ambariService: AmbariService => ambariService
      case serviceWithConfig: Service with ConfigurationContributor =>
        new AmbariService {
          override val name: String = serviceWithConfig.name
          override val components: Seq[ComponentDescription] = serviceWithConfig.components
          override def contributions(properties: ConfigProperties): ConfigurationBundle =
            serviceWithConfig.contributions(properties)
        }
      case _ => new ServiceWithConfigurationFile {
        override val components: Seq[ComponentDescription] = service.components
        override val name: String = service.name
        override protected val configPath: String = configDirectoryPath
        override def toString: String = s"FileConfigDecoratedService[${service.toString}}]"
      }
    }
}
