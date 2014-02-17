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

class HiveTest extends FlatSpec with MustMatchers {

  "The Hive service" must "have a server, a metastore, a client and a mysql server" in {
    val description = Hive
    description.name must equal("HIVE")
    description.components must (
      have length 4 and
        contain(ComponentDescription.masterComponent("HIVE_SERVER")) and
        contain(ComponentDescription.masterComponent("HIVE_METASTORE")) and
        contain(ComponentDescription.masterComponent("HIVE_CLIENT").makeClient) and
        contain(ComponentDescription.masterComponent("MYSQL_SERVER")))
  }
}
