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

package es.tid.cosmos.api.controllers.admin

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class InMemoryMaintenanceStatusTest extends FlatSpec with MustMatchers {

  val emptyConfig = ConfigFactory.parseString("")

  "In memory maintenance status" must "start in normal mode by default" in {
    new InMemoryMaintenanceStatus(emptyConfig).underMaintenance must be (false)
  }

  it must "be initialized with a configuration setting" in {
    val maintenanceConfig = ConfigFactory.parseString("application.underMaintenance=true")
    new InMemoryMaintenanceStatus(maintenanceConfig).underMaintenance must be (true)
  }
  
  it must "remember entering and leaving maintenance mode" in {
    val status = new InMemoryMaintenanceStatus(emptyConfig)
    status.enterMaintenance()
    status.underMaintenance must be (true)
    status.leaveMaintenance()
    status.underMaintenance must be (false)
  }
}
