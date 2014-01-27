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

class ServiceDependenciesTest extends FlatSpec with MustMatchers {

  import ServiceDependencies._

  "Hdfs" must "depend on Zookeeper" in {
    Seq(Hdfs).withDependencies must be (Seq(Zookeeper, Hdfs))
  }

  "MapReduce2" must "depend on YARN" in {
    Seq(MapReduce2).withDependencies must be (Seq(Yarn, MapReduce2))
  }

  "Hive" must "depend on HCatalog and WebHCat" in {
    Seq(Hive).withDependencies must be (Seq(HCatalog, WebHCat, Hive))
  }

  "Service dependencies" must "be added to a given service collection " +
    "with each service placed after its dependencies" in {
    Seq(Hdfs, Hive, MapReduce2).withDependencies must be
    Seq(Zookeeper, Hdfs, HCatalog, WebHCat, Hive, Yarn, MapReduce2)
  }
}
