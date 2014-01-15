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
import es.tid.cosmos.servicemanager.ambari.services.{MapReduce, Hdfs}

class ConfiguratorTestHelpers(masterName: String, slaveCount: Int) {
  import ConfiguratorTestHelpers._

  val dynamicProperties = Map(
    ConfigurationKeys.MasterNode -> masterName,
    ConfigurationKeys.MaxMapTasks -> "20",
    ConfigurationKeys.MaxReduceTasks -> "10",
    ConfigurationKeys.HdfsReplicationFactor -> Math.min(3, slaveCount).toString,
    ConfigurationKeys.MappersPerSlave -> "8",
    ConfigurationKeys.ReducersPerSlave -> "4"
  )

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
    AmbariServiceManager.BasicHadoopServices.map(getProperties).reduce(_++_)
  }
}

object ConfiguratorTestHelpers {
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
