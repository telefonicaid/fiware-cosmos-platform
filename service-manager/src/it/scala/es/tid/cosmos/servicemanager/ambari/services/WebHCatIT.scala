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


import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys

class WebHCatIT extends ConfiguredServiceTest {

  override val dynamicProperties = Map(
    ConfigurationKeys.MasterNode -> "aMasterNodeName",
    ConfigurationKeys.ZookeeperHosts -> Seq("hostname1:1234", "hostname2:1234").mkString(",")
  )

  override val service = WebHCat

  "A WebHCat service" must "have global and service configuration contributions without core" in {
    contributions.global must be('defined)
    contributions.core must not be('defined)
    contributions.services must have length 1
  }

  it must "have the zookeeper hosts injected as dynamic properties" in {
    contributions.services.head
      .properties("templeton.zookeeper.hosts") must equal("hostname1:1234,hostname2:1234")
  }
}
