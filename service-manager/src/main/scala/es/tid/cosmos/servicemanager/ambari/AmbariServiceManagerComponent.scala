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

import es.tid.cosmos.platform.common.ConfigComponent
import es.tid.cosmos.platform.ial.InfrastructureProviderComponent
import es.tid.cosmos.servicemanager.ambari.rest.AmbariServer
import es.tid.cosmos.servicemanager._

trait AmbariServiceManagerComponent extends ServiceManagerComponent {
  this: InfrastructureProviderComponent with ConfigComponent =>

  lazy val serviceManager: ServiceManager = {
    new AmbariServiceManager(
      new AmbariServer(
        config.getString("ambari.server.url"),
        config.getInt("ambari.server.port"),
        config.getString("ambari.server.username"),
        config.getString("ambari.server.password")),
      infrastructureProvider,
      config.getInt("ambari.servicemanager.refresh.graceperiod.seconds") * 1000,
      ClusterId(config.getString("hdfs.cluster.id"))
    )
  }
}
