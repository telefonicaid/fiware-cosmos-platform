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

  "Service dependencies" must "be added to a given service collection " +
    "with each service placed after its dependencies" in {
    Seq(Hive).withDependencies must be (Seq(HCatalog, WebHCat, Hive))
    Seq(Hdfs, Hive, MapReduce2).withDependencies must be
      Seq(Hdfs, HCatalog, WebHCat, Hive, MapReduce2)
  }
}
