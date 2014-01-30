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

import scala.concurrent.duration._
import scala.language.postfixOps

import es.tid.cosmos.platform.common.ConfigComponent
import es.tid.cosmos.platform.ial.InfrastructureProviderComponent
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.configuration.HadoopConfig
import es.tid.cosmos.servicemanager.ambari.rest.AmbariServer
import es.tid.cosmos.servicemanager.clusters.{ClusterDaoComponent, ClusterId}

trait AmbariServiceManagerComponent extends ServiceManagerComponent {
  this: InfrastructureProviderComponent with ConfigComponent with ClusterDaoComponent =>

  private val ambariServer = new AmbariServer(
    config.getString("ambari.server.url"),
    config.getInt("ambari.server.port"),
    config.getString("ambari.server.username"),
    config.getString("ambari.server.password"))
  override lazy val serviceManager: AmbariServiceManager = new AmbariServiceManager(
      ambariServer,
      infrastructureProvider,
      ClusterId(config.getString("hdfs.cluster.id")),
      config.getInt("ambari.servicemanager.exclusiveMasterSizeCutoff"),
      HadoopConfig(
        mrAppMasterMemory = config.getInt("ambari.servicemanager.mrAppMasterMemory"),
        mapTaskMemory = config.getInt("ambari.servicemanager.mapTaskMemory"),
        mapHeapMemory = config.getInt("ambari.servicemanager.mapHeapMemory"),
        mappersPerSlave = config.getInt("ambari.servicemanager.mappersPerSlave"),
        reduceTaskMemory = config.getInt("ambari.servicemanager.reduceTaskMemory"),
        reduceHeapMemory = config.getInt("ambari.servicemanager.reduceHeapMemory"),
        reducersPerSlave = config.getInt("ambari.servicemanager.reducersPerSlave"),
        yarnTotalMemory = config.getInt("ambari.servicemanager.yarnTotalMemory"),
        yarnContainerMinimumMemory = config
          .getInt("ambari.servicemanager.yarnContainerMinimumMemory"),
        yarnVirtualToPhysicalMemoryRatio = config
          .getDouble("ambari.servicemanager.yarnVirtualToPhysicalMemoryRatio"),
        zookeeperPort = config.getInt("ambari.servicemanager.zookeeperPort")
      ),
      new AmbariClusterDao(
        clusterDao,
        ambariServer,
        AmbariServiceManager.AllServices,
        config.getInt("ambari.servicemanager.initialization.graceperiod.minutes") minutes)
    )
}
