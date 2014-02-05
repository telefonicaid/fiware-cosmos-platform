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

class InfinityfsDriverTest extends FlatSpec with MustMatchers {

  "An infinity service" must "have a single driver component" in {
    InfinityfsDriver.components must be (Seq(
      ComponentDescription.allNodesComponent("INFINITY_HFS_DRIVER").makeClient
    ))
  }

  val dynamicProperties = Map(
    ConfigurationKeys.InfinityMasterNode -> "andromeda-infinity"
  )
  val contributedConfig = InfinityfsDriver.contributions(dynamicProperties).core.get

  it must "have the default infinity's cluster configured" in {
    contributedConfig.properties("fs.infinity.defaultAuthority") must be ("andromeda-infinity:8020")
  }

  it must "configure the infinity HFS driver" in {
    contributedConfig.properties("fs.infinity.impl").toString must include ("InfinityFileSystem")
  }
}
