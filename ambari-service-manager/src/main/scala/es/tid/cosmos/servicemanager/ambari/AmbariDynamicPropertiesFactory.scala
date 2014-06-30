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

import es.tid.cosmos.servicemanager.configuration.DynamicPropertiesFactory
import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys._
import es.tid.cosmos.servicemanager.ambari.configuration.HadoopConfig

/** Factory for run-time, dynamic cluster configuration.
  *
  * @constructor
  * @param hadoopConfig        the hadoop configuration.
  * @param infinityMasterName  gets the current infinity master node name when called
  */
class AmbariDynamicPropertiesFactory(
     hadoopConfig: HadoopConfig,
     infinityMasterName: () => Option[String]) extends DynamicPropertiesFactory {

  override def forCluster(masterName: String, slaveNames: Seq[String]): ConfigProperties = Map(
    MasterNode -> masterName,

    InfinityMasterNode -> infinityMasterName().getOrElse(""),

    HdfsReplicationFactor -> Math.min(3, slaveNames.length).toString,
    NameNodeHttpPort -> hadoopConfig.nameNodeHttpPort.toString,

    MrAppMasterMemory -> hadoopConfig.mrAppMasterMemory.toString,

    MapTaskMemory -> hadoopConfig.mapTaskMemory.toString,
    MapHeapMemory -> hadoopConfig.mapHeapMemory.toString,
    MaxMapTasks -> (hadoopConfig.mappersPerSlave * slaveNames.length).toString,

    ReduceTaskMemory -> hadoopConfig.reduceTaskMemory.toString,
    ReduceHeapMemory -> hadoopConfig.reduceHeapMemory.toString,
    MaxReduceTasks -> (1.75 * hadoopConfig.reducersPerSlave * slaveNames.length).round.toString,

    YarnTotalMemory -> hadoopConfig.yarnTotalMemory.toString,
    YarnContainerMinimumMemory -> hadoopConfig.yarnContainerMinimumMemory.toString,
    YarnVirtualToPhysicalMemoryRatio -> hadoopConfig.yarnVirtualToPhysicalMemoryRatio.toString,

    ZookeeperHosts -> zookeeperHosts(slaveNames, hadoopConfig.zookeeperPort).mkString(","),
    ZookeeperPort -> hadoopConfig.zookeeperPort.toString
  )

  private def zookeeperHosts(hosts: Seq[String], port: Int) = hosts.map(host => s"$host:$port")
}
