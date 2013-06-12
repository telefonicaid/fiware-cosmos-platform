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

class HdfsTest extends FlatSpec with MustMatchers {
  "An Hdfs service" must "have a namenode, datanode and hdfs client" in {
    val description = Hdfs
    description.name must equal("HDFS")
    description.components must (
      have length (3) and
      contain(ComponentDescription("NAMENODE", isMaster = true)) and
      contain(ComponentDescription("DATANODE", isMaster = false)) and
      contain(ComponentDescription("HDFS_CLIENT", isMaster = true)))
    val contributions = description.contributions("aNameNodeName")
    contributions.global must be('defined)
    contributions.core must be('defined)
    contributions.services must have length(1)
  }

  it must "return the namenode port" in {
    Hdfs.nameNodeHttpPort must be (50070)
  }
}
