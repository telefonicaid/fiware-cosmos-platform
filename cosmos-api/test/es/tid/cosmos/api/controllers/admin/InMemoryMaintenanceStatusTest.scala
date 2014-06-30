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

package es.tid.cosmos.api.controllers.admin

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class InMemoryMaintenanceStatusTest extends FlatSpec with MustMatchers {

  val emptyConfig = ConfigFactory.parseString("")

  "In memory maintenance status" must "start in normal mode by default" in {
    new InMemoryMaintenanceStatus(emptyConfig) must not be 'underMaintenance
  }

  it must "be initialized with a configuration setting" in {
    val maintenanceConfig = ConfigFactory.parseString("application.underMaintenance=true")
    new InMemoryMaintenanceStatus(maintenanceConfig) must be ('underMaintenance)
  }
  
  it must "remember entering and leaving maintenance mode" in {
    val status = new InMemoryMaintenanceStatus(emptyConfig)
    status.enterMaintenance()
    status must be ('underMaintenance)
    status.leaveMaintenance()
    status must not be 'underMaintenance
  }
}
