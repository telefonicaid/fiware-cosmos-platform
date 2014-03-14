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

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.configuration._

/** Factory object to turn service descriptions to ambari-specific ones. */
object AmbariServiceDescriptionFactory {

  def toAmbariService(instance: AnyServiceInstance, configDirectoryPath: String): AmbariService =
    toAmbariService(instance.service, configDirectoryPath)

  /** Turn a service to an `AmbariServiceDescription`.
    * If the service is found not to be a `ConfigurationContributor` it wraps it with a
    * `ServiceWithConfigurationFile`.
    *
    * If the given service is already an `AmbariServiceDescription` it will simply return it.
    *
    * @param service            the service to decorate
    * @param configDirectoryPath the directory path to find the service configuration file
    */
  def toAmbariService(service: Service, configDirectoryPath: String): AmbariService =
    service match {
      case ambariService: AmbariService => ambariService
      case serviceWithConfig: Service with ConfigurationContributor =>
        new AmbariService with NoParametrization {
          override val name: String = serviceWithConfig.name
          override def contributions(properties: ConfigProperties): ConfigurationBundle =
            serviceWithConfig.contributions(properties)
          override def ambariService: AmbariServiceDetails = service.ambariService
        }
      case _ => new ServiceWithConfigurationFile with NoParametrization {
        override val name: String = service.name
        override protected val configPath: String = configDirectoryPath
        override def toString: String = s"FileConfigDecoratedService[${service.toString}}]"
        override def ambariService: AmbariServiceDetails = service.ambariService
      }
    }
}
