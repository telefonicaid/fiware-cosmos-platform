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

class ZookeeperTest extends FlatSpec with MustMatchers {

  "A Zookeeper service" must "have a zookeeper server and a client" in {
    val description = Zookeeper
    description.name must equal("ZOOKEEPER")
    description.components must (
      have length 2 and
      contain(ComponentDescription("ZOOKEEPER_SERVER", isMaster = false)) and
      contain(ComponentDescription("ZOOKEEPER_CLIENT", isMaster = true, isClient = true))
    )
    val contributions = description.contributions(Map.empty)
    contributions.global must be('defined)
    contributions.core must not be('defined)
    contributions.services must be('empty)
  }
}
