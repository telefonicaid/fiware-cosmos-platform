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

import es.tid.cosmos.servicemanager.ambari.rest.Host
import es.tid.cosmos.servicemanager.ambari.configuration.{ConfigurationKeys, HadoopConfig}


/** Factory for run-time, dynamic cluster configuration. */
object DynamicProperties {

  /** Generate dynamic configuration properties for a given cluster.
    * This is useful for properties that differ from cluster to cluster such as host names.
    *
    * @param hadoopConfig the hadoop configuration.
    * @param master       the cluster master node
    * @param slaves       the cluster slave nodes
    * @return             the dynamically generated configuration properties
    * @see [[es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys]]
    */
  def apply(hadoopConfig: HadoopConfig, master: Host, slaves: Seq[Host])
      : Map[ConfigurationKeys.Value, String] = Map(
    ConfigurationKeys.HdfsReplicationFactor -> Math.min(3, slaves.length).toString,
    ConfigurationKeys.MasterNode -> master.name,
    ConfigurationKeys.MappersPerSlave -> hadoopConfig.mappersPerSlave.toString,
    ConfigurationKeys.MaxMapTasks ->
      (hadoopConfig.mappersPerSlave * slaves.length).toString,
    ConfigurationKeys.MaxReduceTasks ->
      (1.75 * hadoopConfig.reducersPerSlave * slaves.length).round.toString,
    ConfigurationKeys.ReducersPerSlave -> hadoopConfig.reducersPerSlave.toString,
    ConfigurationKeys.ZookeeperHosts -> zookeeperHosts(slaves, hadoopConfig.zookeeperPort)
      .mkString(","),
    ConfigurationKeys.ZookeeperPort -> hadoopConfig.zookeeperPort.toString
  )

  private def zookeeperHosts(hosts: Seq[Host], port: Int) = hosts.map(h => s"${h.name}:$port")
}
