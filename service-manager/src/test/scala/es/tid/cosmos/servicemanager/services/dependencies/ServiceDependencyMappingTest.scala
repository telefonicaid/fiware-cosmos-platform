/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.servicemanager.services.dependencies

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.services._

class ServiceDependencyMappingTest extends FlatSpec with MustMatchers {

  val testMapping = new ServiceDependencyMapping(ServiceCatalogue)

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
    val requiredDependency: Service = InfinityDriver
    dependencies(Hdfs) must contain (requiredDependency)
    dependencies(MapReduce2) must contain (requiredDependency)
    dependencies(Yarn) must contain (requiredDependency)
  }

  private def dependencies(services: Service*) = testMapping.executionPlan(services.toSet)
}
