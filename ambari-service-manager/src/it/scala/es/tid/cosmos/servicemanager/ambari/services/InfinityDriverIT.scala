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

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys.InfinityMasterNode
import es.tid.cosmos.servicemanager.services.InfinityDriver.InfinityDriverParameters

class InfinityDriverIT extends ConfiguredServiceTest {

  override val dynamicProperties = Map(
    InfinityMasterNode -> "andromeda-infinity"
  )

  override def configurator = AmbariInfinityDriver.configurator(
    InfinityDriverParameters("secret"), resourcesConfigDirectory)

  val contributedConfig = contributions.core.get
  val serviceConfig = contributions.services.head

  "An infinity service" must "only have core and service contributions" in {
    contributions.global must not be 'defined
    contributions.core must be ('defined)
    contributions.services must have length (1)
  }

  it must "have the default infinity's cluster configured" in {
    contributedConfig.properties("fs.infinity.defaultAuthority").toString must include ("andromeda-infinity")
  }

  it must "configure the infinity HFS driver" in {
    contributedConfig.properties("fs.infinity.impl").toString must include ("InfinityFileSystem")
  }

  it must "provide a list of ports to be blocked" in {
    serviceConfig.properties("blocked_ports").toString must include ("8088")
  }
}
