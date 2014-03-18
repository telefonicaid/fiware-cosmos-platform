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

package es.tid.cosmos.servicemanager.services.dependencies

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.services._

class ServiceDependencyMappingTest extends FlatSpec with MustMatchers {

  val testMapping = new ServiceDependencyMapping(ServiceDependencies.ServiceCatalogue)

  "Hdfs" must "depend on Zookeeper" in {
    dependencies(Hdfs) must contain (Zookeeper: Service)
  }

  "MapReduce2" must "depend on YARN" in {
    dependencies(MapReduce2) must contain (Yarn: Service)
  }

  "Hive" must "depend on HCatalog and WebHCat" in {
    dependencies(Hive) must (contain(HCatalog: Service) and contain(WebHCat: Service))
  }

  "Cosmos user service" must "depend on HDFS" in {
    dependencies(CosmosUserService) must contain (Hdfs: Service)
  }

  "Service dependencies" must "be added to a given service collection " +
    "with each service placed after its dependencies" in {
    dependencies(Hdfs, Hive, MapReduce2) must be
      Seq(Zookeeper, Hdfs, HCatalog, WebHCat, Hive, Yarn, MapReduce2)
  }

  "Services potentially using infinity" must "depend on infinity FS driver" in {
    val requiredDependency: Service = InfinityfsDriver
    dependencies(Hdfs) must contain (requiredDependency)
    dependencies(MapReduce2) must contain (requiredDependency)
    dependencies(Yarn) must contain (requiredDependency)
  }

  private def dependencies(services: Service*) = testMapping.executionPlan(services.toSet)
}
