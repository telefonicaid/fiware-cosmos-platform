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

import es.tid.cosmos.servicemanager.ServiceDescription
import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescription
import es.tid.cosmos.servicemanager.ambari.services.dependencies.ServiceDependencies._

class ConfiguratorTestHelpers(
    masterName: String,
    slaveNames: Seq[String],
    hadoopConfig: HadoopConfig = ConfiguratorTestHelpers.TestHadoopConfig) {
  import ConfiguratorTestHelpers._

  val dynamicPropertiesFactory = new DynamicPropertiesFactory(hadoopConfig, () => None)

  val dynamicProperties = dynamicPropertiesFactory.forCluster(masterName, slaveNames)

  private def propertiesUpTo(confType: String, number: Int) =
    (1 to number).map(properties(confType, _)).reduce(_++_)

  def mergedCoreConfiguration(number: Int) =
    new CoreConfiguration(propertiesUpTo("Core", number))

  def mergedGlobalConfiguration(number: Int, instance: AmbariClusterManager) =
    GlobalConfiguration(instance.contributions(dynamicProperties).global.get.properties ++
      propertiesUpTo("Global", number))
}

object ConfiguratorTestHelpers {

  val TestHadoopConfig = HadoopConfig(
    mrAppMasterMemory = 100,
    mapTaskMemory = 200,
    mapHeapMemory = 100,
    mappersPerSlave = 8,
    reduceTaskMemory = 200,
    reduceHeapMemory = 100,
    reducersPerSlave = 4,
    yarnTotalMemory = 1024,
    yarnContainerMinimumMemory = 100,
    yarnVirtualToPhysicalMemoryRatio = 2.1,
    nameNodeHttpPort = 50070,
    zookeeperPort = 1234,
    servicesConfigDirectory = "/tmp/services"
  )

  val BasicServices = AmbariServiceManager.BasicHadoopServices.withDependencies
    .map(toAmbariServicesNoConfig)

  val AllServices = AmbariServiceManager.AllServices.withDependencies.map(toAmbariServicesNoConfig)

  def toAmbariServicesNoConfig(service: ServiceDescription): AmbariServiceDescription =
    service match {
      case ambariService: AmbariServiceDescription => ambariService
      case _ => new AmbariServiceDescription with NoConfigurationContribution {
        override val name = service.name
        override val components = service.components
      }
    }

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
