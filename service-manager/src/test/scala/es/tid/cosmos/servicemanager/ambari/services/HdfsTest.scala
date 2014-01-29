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

package es.tid.cosmos.servicemanager.ambari.services

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.ComponentDescription
import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys

class HdfsTest extends FlatSpec with MustMatchers {

  val dynamicProperties = Map(
    ConfigurationKeys.MasterNode -> "aMasterNodeName",
    ConfigurationKeys.HdfsReplicationFactor -> "3"
  )

  "An Hdfs service" must "have a namenode, datanode and hdfs client" in {
    val description = Hdfs
    description.name must equal("HDFS")
    description.components must (
      have length 3 and
      contain(ComponentDescription("NAMENODE", isMaster = true, isClient = false)) and
      contain(ComponentDescription("DATANODE", isMaster = false, isClient = false)) and
      contain(ComponentDescription("HDFS_CLIENT", isMaster = true, isClient = true)))
    val contributions = description.contributions(dynamicProperties)
    contributions.global must be('defined)
    contributions.core must be('defined)
    contributions.services must have length 1
  }

  it must "return the namenode port" in {
    Hdfs.nameNodeHttpPort must be (50070)
  }

  /* Note: This is a HDFS-specific configuration needed even when Oozie is not installed */
  it must "have the oozie proxyuser group configured to be [cosmos]" in {
    Hdfs.contributions(dynamicProperties).core.get.properties(
      "hadoop.proxyuser.oozie.groups") must equal("cosmos")
  }

  /* Note: This is a HDFS-specific configuration needed even when Oozie is not installed */
  it must "have the oozie proxyuser hosts configured to be [*]" in {
    Hdfs.contributions(dynamicProperties).core.get.properties(
      "hadoop.proxyuser.oozie.hosts") must equal("*")
  }
}
