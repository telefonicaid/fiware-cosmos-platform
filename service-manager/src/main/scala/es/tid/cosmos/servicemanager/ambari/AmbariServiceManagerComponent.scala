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

package es.tid.cosmos.servicemanager.ambari

import com.typesafe.config.ConfigFactory

import es.tid.cosmos.platform.manager.ial.InfrastructureProviderComponent
import es.tid.cosmos.servicemanager.ambari.rest.AmbariServer
import es.tid.cosmos.servicemanager.{ServiceManager, ServiceManagerComponent}

trait AmbariServiceManagerComponent extends ServiceManagerComponent {
  this: InfrastructureProviderComponent =>

  lazy val serviceManager: ServiceManager = {
    val config = ConfigFactory.load()
    new AmbariServiceManager(
      new AmbariServer(
        config.getString("ambari.server.url"),
        config.getInt("ambari.server.port"),
        config.getString("ambari.server.username"),
        config.getString("ambari.server.password")),
      infrastructureProvider)
  }
}
