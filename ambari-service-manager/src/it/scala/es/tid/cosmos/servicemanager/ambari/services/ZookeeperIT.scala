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

import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys

class ZookeeperIT extends ConfiguredServiceTest {

  override val dynamicProperties = Map(ConfigurationKeys.ZookeeperPort -> "1234")

  override def configurator = AmbariZookeeper.configurator(None, resourcesConfigDirectory)

  "A Zookeeper service" must "only have global configuration contributions" in {
    contributions.global must be ('defined)
    contributions.core must not be 'defined
    contributions.services must be ('empty)
  }

  it must "have the zookeeper server port injected as dynamic properties" in {
    contributions.global.get.properties("clientPort") must equal("1234")
  }
}
