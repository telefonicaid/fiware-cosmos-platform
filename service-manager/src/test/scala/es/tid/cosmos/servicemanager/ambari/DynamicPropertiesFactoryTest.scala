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

import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys._

class DynamicPropertiesFactoryTest extends FlatSpec with MustMatchers with MockitoSugar
    with OneInstancePerTest {

  val hadoopConfig = ConfiguratorTestHelpers.TestHadoopConfig
  val masterName = "masterNode"

  "Dynamic HDFS replication factor" must "be equal to the number of slaves on clusters " +
    "with up to 3 slaves" in {
    dynamicProperties(size = 1)(HdfsReplicationFactor) must be ("1")
    dynamicProperties(size = 2)(HdfsReplicationFactor) must be ("2")
    dynamicProperties(size = 3)(HdfsReplicationFactor) must be ("3")
  }


  it must "be equal to 3 on clusters with more than 3 slaves" in {
    dynamicProperties(size = 4)(HdfsReplicationFactor) must be ("3")
    dynamicProperties(size = 100)(HdfsReplicationFactor) must be ("3")
  }

  "Dynamic master node" must "be the cluster's master node hostname" in {
    dynamicProperties(size = 2)(MasterNode) must be (masterName)
  }

  "Dynamic zookeeper hosts" must "be the cluster's slaves using the zookeeper port" in {
    dynamicProperties(size = 2)(ZookeeperHosts) must be ("slave1:1234,slave2:1234")
  }

  "Dynamic maximum map tasks for a cluster" must "be the sum of mappers of all slaves" in {
    dynamicProperties(size = 2)(MaxMapTasks) must be ("16")
  }

  "Dynamic maximum reduce tasks for a cluster" must "be 1.75 * reducers of all slaves" in {
    dynamicProperties(size = 2)(MaxReduceTasks) must be ("14")
  }

  "Infinity master node" must "point to infinity when present" in {
    val properties = dynamicProperties(size = 1, infinityMasterName = Some("andromeda-infinity"))
    properties(InfinityMasterNode) must be ("andromeda-infinity")
  }

  it must "be empty otherwise" in {
    dynamicProperties(size = 1)(InfinityMasterNode) must be ("")
  }

  def dynamicProperties(size: Int, infinityMasterName: Option[String] = None) =
    new DynamicPropertiesFactory(hadoopConfig, () => infinityMasterName)
      .forCluster(masterName, slaveNames(size))

  def slaveNames(size: Int): Seq[String] = (1 to size).map(index => s"slave$index")
}
