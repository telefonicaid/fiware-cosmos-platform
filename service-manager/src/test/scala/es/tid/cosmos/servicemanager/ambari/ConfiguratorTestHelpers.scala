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

import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.ambari.services.ServiceDependencies._

class ConfiguratorTestHelpers(
    masterName: String, slaveCount: Int, withMasterAsSlave: Boolean = true) {
  import ConfiguratorTestHelpers._

  val dynamicProperties = {
    import ConfigurationKeys._
    Map(
      HdfsReplicationFactor -> Math.min(3, slaveCount).toString,
      MasterNode -> masterName,
      MapTaskMemory -> TestHadoopConfig.mapTaskMemory.toString,
      MapHeapMemory -> TestHadoopConfig.mapHeapMemory.toString,
      ReduceTaskMemory -> TestHadoopConfig.reduceTaskMemory.toString,
      ReduceHeapMemory -> TestHadoopConfig.reduceHeapMemory.toString,
      MrAppMasterMemory -> TestHadoopConfig.mrAppMasterMemory.toString,
      YarnTotalMemory -> TestHadoopConfig.yarnTotalMemory.toString,
      YarnContainerMinimumMemory -> TestHadoopConfig.yarnContainerMinimumMemory.toString,
      YarnVirtualToPhysicalMemoryRatio -> TestHadoopConfig.yarnVirtualToPhysicalMemoryRatio.toString,
      ZookeeperHosts -> (if (withMasterAsSlave) 1 to slaveCount else 2 to slaveCount + 1)
        .map(index => s"hostname$index:${TestHadoopConfig.zookeeperPort}").mkString(","),
      ZookeeperPort -> TestHadoopConfig.zookeeperPort.toString
    )
  }

  private def propertiesUpTo(confType: String, number: Int) =
    (1 to number).map(properties(confType, _)).reduce(_++_)

  def mergedCoreConfiguration(number: Int) =
    new CoreConfiguration(propertiesUpTo("Core", number) ++ getConfigurationFromCompulsoryServices(_.core))

  def mergedGlobalConfiguration(number: Int, instance: AmbariServiceManager) =
    GlobalConfiguration(instance.clusterDeployer.contributions(dynamicProperties).global.get.properties ++
      propertiesUpTo("Global", number) ++ getConfigurationFromCompulsoryServices(_.global))

  private def getConfigurationFromCompulsoryServices(extractor: ConfigurationBundle => Option[Configuration]) = {
    def getProperties(contributor: ConfigurationContributor) = extractor(contributor.contributions(dynamicProperties))
      .map(_.properties).getOrElse(Map())
    AmbariServiceManager.BasicHadoopServices.withDependencies.map(getProperties).reduce(_++_)
  }
}

object ConfiguratorTestHelpers {
  
  val TestHadoopConfig = HadoopConfig(
      yarnTotalMemory = 1024,
      yarnContainerMinimumMemory = 100,
      yarnVirtualToPhysicalMemoryRatio = 2.1,
      mapTaskMemory = 200,
      mapHeapMemory = 100,
      reduceTaskMemory = 200,
      reduceHeapMemory = 100,
      mrAppMasterMemory = 100,
      zookeeperPort = 1234
    )
  
  def properties(confType: String, number: Int) =
    Map(s"some${confType}Content$number" -> s"somevalue$number")

  def contributionsWithNumber(number: Int) = {
    ConfigurationBundle(
      Some(GlobalConfiguration(properties("Global", number))),
      Some(CoreConfiguration(properties("Core", number))),
      List(ServiceConfiguration(s"service-site$number", properties("Service", number))))
  }

  def expectedConfigurationOf(number: Int) = properties("Service", number)
}
