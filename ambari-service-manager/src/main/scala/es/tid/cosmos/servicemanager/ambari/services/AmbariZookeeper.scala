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

import es.tid.cosmos.servicemanager.services.Zookeeper

object AmbariZookeeper extends AmbariService with FileConfiguration {
  override val service = Zookeeper
  override val components: Seq[ComponentDescription] = Seq(
    /* Zookeeper is a distributed application.
     * We choose to have one zookeeper server on each slave for resilience purposes.
     */
    ComponentDescription.slaveComponent("ZOOKEEPER_SERVER"),
    ComponentDescription.masterComponent("ZOOKEEPER_CLIENT").makeClient
  )
}
