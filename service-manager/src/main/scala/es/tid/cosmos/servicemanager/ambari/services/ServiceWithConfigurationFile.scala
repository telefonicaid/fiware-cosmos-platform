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

import es.tid.cosmos.servicemanager.ambari.configuration.FileConfigurationContributor
import es.tid.cosmos.servicemanager.{ComponentDescription, ServiceDescription}

/** Trait for service descriptions that have a configuration file. */
trait ServiceWithConfigurationFile
  extends AmbariServiceDescription with FileConfigurationContributor {

  override protected lazy val configName = name.toLowerCase
}

/** Companion object with utils to decorate services with file configuration. */
object ServiceWithConfigurationFile {
  /** Turn a service to an `AmbariServiceDescription` by decorating it with a
    * `ServiceWithConfigurationFile`.
    *
    * If the given service is already an `AmbariServiceDescription` it will simply return it
    * since it can provide its own configuration.
    * 
    * @param service                    the service to decorate
    * @param configDirectoryPath the directory path to find the service configuration file
    */
  def decorateWithFileConfiguration(service: ServiceDescription)
                                   (implicit configDirectoryPath: String)
      : AmbariServiceDescription =
    service match {
      case ambariService: AmbariServiceDescription => ambariService
      case _ => new ServiceWithConfigurationFile {
        override val components: Seq[ComponentDescription] = service.components
        override val name: String = service.name
        override protected val configPath: String = configDirectoryPath
        override def toString: String = s"FileConfigDecoratedService[${service.toString}}]"
      }
    }
}
