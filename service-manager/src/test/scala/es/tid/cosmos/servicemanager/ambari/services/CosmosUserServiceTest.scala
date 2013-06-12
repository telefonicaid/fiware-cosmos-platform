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

import es.tid.cosmos.servicemanager.{ClusterUser, ComponentDescription}
import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys

class CosmosUserServiceTest extends FlatSpec with MustMatchers {
  "A Cosmos user service" must "have a master and a slave components" in {
    val description = new CosmosUserService(Seq(ClusterUser("userName", "public_key")))
    description.name must equal("COSMOS_USER")
    description.components must (
      have length (2) and
      contain(ComponentDescription("USER_MASTER_MANAGER", isMaster = true)) and
      contain(ComponentDescription("USER_SLAVE_MANAGER", isMaster = false))
    )
    val contributions = description.contributions(
      Map(ConfigurationKeys.MasterNode -> "aNameNodeName"))
    contributions.global must be('empty)
    contributions.core must be('empty)
    contributions.services must have length(1)
  }
}
