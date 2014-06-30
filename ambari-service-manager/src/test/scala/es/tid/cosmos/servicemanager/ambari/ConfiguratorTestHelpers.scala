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

import es.tid.cosmos.servicemanager.ambari.ConfiguratorTestHelpers._
import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.ambari.services.{AmbariService, AmbariServiceFactory}
import es.tid.cosmos.servicemanager.configuration._
import es.tid.cosmos.servicemanager.services.ServiceCatalogue

class ConfiguratorTestHelpers(
    masterName: String,
    slaveNames: Seq[String],
    hadoopConfig: HadoopConfig = ConfiguratorTestHelpers.TestHadoopConfig) {
  val dynamicPropertiesFactory = new AmbariDynamicPropertiesFactory(hadoopConfig, () => None)
  val dynamicProperties = dynamicPropertiesFactory.forCluster(masterName, slaveNames)

  private def propertiesUpTo(confType: String, number: Int) =
    (1 to number).map(properties(confType, _)).reduce(_++_)

  def mergedCoreConfiguration(number: Int) =
    new CoreConfiguration(propertiesUpTo("Core", number))

  def mergedGlobalConfiguration(number: Int, instance: AmbariClusterManager) = GlobalConfiguration(
    instance.globalConfiguration.contributions(dynamicProperties).global.get.properties ++
      propertiesUpTo("Global", number)
  )
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

  val AllServices: Set[AmbariService] = ServiceCatalogue.map(AmbariServiceFactory.lookup)

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
