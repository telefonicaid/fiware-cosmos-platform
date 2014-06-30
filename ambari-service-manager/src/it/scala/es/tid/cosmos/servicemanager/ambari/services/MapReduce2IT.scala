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

import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys._

class MapReduce2IT extends ConfiguredServiceTest {

  override val dynamicProperties = Map(
    MasterNode -> "aMasterNodeName",
    MrAppMasterMemory -> "100",
    MapTaskMemory -> "200",
    MapHeapMemory -> "100",
    MaxMapTasks -> "8",
    ReduceTaskMemory -> "100",
    ReduceHeapMemory -> "50",
    MaxReduceTasks -> "4"
  )

  override def configurator = AmbariMapReduce2.configurator((), resourcesConfigDirectory)

  "A MapReduce service" must "have global, core and service configuration contributions" in {
    contributions.global must be ('defined)
    contributions.core must be ('defined)
    contributions.services must have length 1
  }
}
