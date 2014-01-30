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

package es.tid.cosmos.servicemanager.ambari.configuration

object ConfigurationKeys extends Enumeration {
  val HdfsReplicationFactor,
      MasterNode,
      MrAppMasterMemory,

      MapTaskMemory,
      MapHeapMemory,
      MaxMapTasks,

      ReduceTaskMemory,
      ReduceHeapMemory,
      MaxReduceTasks,

      YarnTotalMemory,
      YarnContainerMinimumMemory,
      YarnVirtualToPhysicalMemoryRatio,

      ZookeeperHosts,
      ZookeeperPort = Value
}
