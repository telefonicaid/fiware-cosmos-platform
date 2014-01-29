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
import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys

class WebHCatTest extends FlatSpec with MustMatchers {

  val dynamicProperties = Map(
    ConfigurationKeys.MasterNode -> "aMasterNodeName",
    ConfigurationKeys.ZookeeperHosts -> Seq("hostname1:1234", "hostname2:1234").mkString(",")
  )

  "A WebHCat service" must "have a server as a component" in {
    val description = WebHCat
    description.name must equal("WEBHCAT")
    description.components must (
      have length 1 and contain(ComponentDescription("WEBHCAT_SERVER", isMaster = true))
    )
    val contributions = description.contributions(dynamicProperties)
    contributions.global must be('defined)
    contributions.core must not be('defined)
    contributions.services must have length 1
  }

  it must "have the zookeeper hosts injected as dynamic properties" in {
    WebHCat.contributions(dynamicProperties).services.head
      .properties("templeton.zookeeper.hosts") must equal("hostname1:1234,hostname2:1234")
  }
}
