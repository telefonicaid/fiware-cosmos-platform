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

package es.tid.cosmos.servicemanager.ambari.services.dependencies

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.ambari.services._

class ServiceDependenciesTest extends FlatSpec with MustMatchers {

  import ServiceDependencies._

  "Hdfs" must "depend on Zookeeper" in {
    Seq(Hdfs).withDependencies must contain (Zookeeper: Service)
  }

  "MapReduce2" must "depend on YARN" in {
    Seq(MapReduce2).withDependencies must contain (Yarn: Service)
  }

  "Hive" must "depend on HCatalog and WebHCat" in {
    Seq(Hive).withDependencies must (contain(HCatalog: Service) and contain(WebHCat: Service))
  }

  "Cosmos user service" must "depend on HDFS" in {
    Seq(CosmosUserService).withDependencies must contain (Hdfs: Service)
  }

  "Service dependencies" must "be added to a given service collection " +
    "with each service placed after its dependencies" in {
    Seq(Hdfs, Hive, MapReduce2).withDependencies must be
    Seq(Zookeeper, Hdfs, HCatalog, WebHCat, Hive, Yarn, MapReduce2)
  }

  "Services potentially using infinity" must "depend on infinity FS driver" in {
    val requiredDependency: Service = InfinityfsDriver
    Seq(Hdfs).withDependencies must contain (requiredDependency)
    Seq(MapReduce2).withDependencies must contain (requiredDependency)
    Seq(Yarn).withDependencies must contain (requiredDependency)
  }
}
