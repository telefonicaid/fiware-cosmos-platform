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

class HiveTest extends FlatSpec with MustMatchers {
  "The Hive service" must "have a server, a metastore, a client and a mysql server" in {
    val description = Hive
    description.name must equal("HIVE")
    description.components must (
      have length 4 and
        contain(ComponentDescription("HIVE_SERVER", isMaster = true, isClient = false)) and
        contain(ComponentDescription("HIVE_METASTORE", isMaster = true, isClient = false)) and
        contain(ComponentDescription("HIVE_CLIENT", isMaster = true, isClient = true)) and
        contain(ComponentDescription("MYSQL_SERVER", isMaster = true, isClient = false)))
    val contributions = description.contributions(Map(
      ConfigurationKeys.HdfsReplicationFactor -> "3",
      ConfigurationKeys.MasterNode -> "aMasterNodeName",
      ConfigurationKeys.MaxMapTasks -> "10",
      ConfigurationKeys.MaxReduceTasks -> "5"
    ))
    contributions.global must be ('defined)
    contributions.core must be ('defined)
    contributions.services must have length 1
  }
}
