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

import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys._
import es.tid.cosmos.servicemanager.ambari.configuration.{ConfigurationKeys, HadoopConfig}


/** Factory for run-time, dynamic cluster configuration. */
object DynamicProperties {

  /** Generate dynamic configuration properties for a given cluster.
    * This is useful for properties that differ from cluster to cluster such as host names.
    *
    * @param hadoopConfig the hadoop configuration.
    * @param masterName   the cluster master node hostname
    * @param slaveNames   the cluster slave nodes hostnames
    * @return             the dynamically generated configuration properties
    * @see [[es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys]]
    */
  def apply(hadoopConfig: HadoopConfig, masterName: String, slaveNames: Seq[String])
      : Map[ConfigurationKeys.Value, String] = Map(
    HdfsReplicationFactor -> Math.min(3, slaveNames.length).toString,
    MasterNode -> masterName,

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
