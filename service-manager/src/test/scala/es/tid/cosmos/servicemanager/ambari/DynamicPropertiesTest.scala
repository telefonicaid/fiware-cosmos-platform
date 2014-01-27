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

import org.mockito.BDDMockito.given
import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys._
import es.tid.cosmos.servicemanager.ambari.rest.Host

class DynamicPropertiesTest extends FlatSpec with MustMatchers with MockitoSugar
    with OneInstancePerTest {

  val hadoopConfig = ConfiguratorTestHelpers.TestHadoopConfig
  val master = mock[Host]

  "Dynamic HDFS replication factor" must "be equal to the number of slaves on clusters " +
    "with up to 3 slaves" in {
    DynamicProperties(hadoopConfig, master, slaves(1))(HdfsReplicationFactor) must be ("1")
    DynamicProperties(hadoopConfig, master, slaves(2))(HdfsReplicationFactor) must be ("2")
    DynamicProperties(hadoopConfig, master, slaves(3))(HdfsReplicationFactor) must be ("3")
  }

  it must "be equal to 3 on clusters with more than 3 slaves" in {
    DynamicProperties(hadoopConfig, master, slaves(4))(HdfsReplicationFactor) must be ("3")
    DynamicProperties(hadoopConfig, master, slaves(100))(HdfsReplicationFactor) must be ("3")
  }

  "Dynamic master node" must "be the cluster's master node hostname" in {
    val hostname = "MasterHostName"
    given(master.name).willReturn(hostname)
    DynamicProperties(hadoopConfig, master, slaves(2))(MasterNode) must be (hostname)
  }

  "Dynamic zookeeper hosts" must "be the cluster's slaves using the zookeeper port" in {
    val slave1, slave2 = mock[Host]
    given(slave1.name).willReturn("slave1Hostname")
    given(slave2.name).willReturn("slave2Hostname")
    DynamicProperties(hadoopConfig, master, Seq(slave1, slave2))(ZookeeperHosts) must
      be ("slave1Hostname:1234,slave2Hostname:1234")
  }

  def slaves(size: Int): Seq[Host] = (1 to size).map(_ => mock[Host])
}
