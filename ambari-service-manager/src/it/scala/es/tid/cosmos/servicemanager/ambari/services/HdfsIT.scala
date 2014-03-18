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

import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys._
import es.tid.cosmos.servicemanager.services.Hdfs.HdfsParameters

class HdfsIT extends ConfiguredServiceTest {

  override val dynamicProperties = Map(
    MasterNode -> "aMasterNodeName",
    HdfsReplicationFactor -> "3",
    NameNodeHttpPort -> "50070"
  )
  val parameters = HdfsParameters("123")
  override def configurator = AmbariHdfs.configurator(parameters, resourcesConfigDirectory)

  "An Hdfs service" must "have global, core and service configuration contributions" in {
    contributions.global must be('defined)
    contributions.core must be('defined)
    contributions.services must have length 1
  }

  it must "return the namenode address" in {
    contributions.services.head.properties("dfs.namenode.http-address") must
      be ("aMasterNodeName:50070")
  }

  it must "configure the umask" in {
    contributions.services.head.properties("fs.permissions.umask-mode") must be (parameters.umask)
  }

  /* Note: This is a HDFS-specific configuration needed even when Oozie is not installed */
  it must "have the oozie proxyuser group configured to be [cosmos]" in {
    contributions.core.get.properties("hadoop.proxyuser.oozie.groups") must equal("cosmos")
  }

  /* Note: This is a HDFS-specific configuration needed even when Oozie is not installed */
  it must "have the oozie proxyuser hosts configured to be [*]" in {
    contributions.core.get.properties("hadoop.proxyuser.oozie.hosts") must equal("*")
  }
}
