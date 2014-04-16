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

package es.tid.cosmos.servicemanager.ambari

import scala.concurrent.duration._

import es.tid.cosmos.common.ConfigComponent
import es.tid.cosmos.platform.ial.InfrastructureProviderComponent
import es.tid.cosmos.servicemanager.ServiceManagerComponent
import es.tid.cosmos.servicemanager.clusters.{ClusterId, ClusterDaoComponent}
import es.tid.cosmos.servicemanager.ambari.configuration.HadoopConfig
import es.tid.cosmos.servicemanager.ambari.rest.AmbariServer
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceFactory
import es.tid.cosmos.servicemanager.services.ServiceCatalogue

trait AmbariServiceManagerComponent extends ServiceManagerComponent {
  this: InfrastructureProviderComponent with ConfigComponent with ClusterDaoComponent =>

  private val ambariServer = new AmbariServer(
    config.getString("ambari.server.url"),
    config.getInt("ambari.server.port"),
    config.getString("ambari.server.username"),
    config.getString("ambari.server.password"))

  private val hadoopConfig = HadoopConfig(
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
    nameNodeHttpPort = config.getInt("ambari.servicemanager.nameNodeHttpPort"),
    zookeeperPort = config.getInt("ambari.servicemanager.zookeeperPort"),
    servicesConfigDirectory = config.getString("ambari.servicemanager.servicesConfigDirectory")
  )

  private lazy val ambariClusterManager = new AmbariClusterManager(
    ambariServer,
    infrastructureProvider.rootPrivateSshKey,
    hadoopConfig.servicesConfigDirectory,
    AmbariServiceFactory.lookup
  )

  override lazy val serviceManager: AmbariServiceManager = new AmbariServiceManager(
      ambariClusterManager,
      infrastructureProvider,
      ClusterId(config.getString("hdfs.cluster.id")),
      config.getInt("ambari.servicemanager.exclusiveMasterSizeCutoff"),
      hadoopConfig,
      new AmbariClusterDao(
        serviceManagerClusterDao,
        ambariServer,
        ServiceCatalogue.map(AmbariServiceFactory.lookup),
        config.getInt("ambari.servicemanager.initialization.graceperiod.minutes").minutes
      )
    )
}
