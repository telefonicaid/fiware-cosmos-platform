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

import es.tid.cosmos.servicemanager.ComponentLocation._

/** Description of the type and where to distribute a component.
  *
  * @constructor
  * @param name          Service identifier
  * @param distribution  Where to install the component
  * @param isClient      Whether the component is client-only (no running daemon)
  */
case class ComponentDescription(
    name: String,
    distribution: Set[ComponentLocation],
    isClient: Boolean = false) {

  require(!distribution.isEmpty, "Component must be distributed to at least one type of node")

  def isMaster: Boolean = distribution.contains(Master)

  def isSlave: Boolean = distribution.contains(Slave)

  def makeClient: ComponentDescription = copy(isClient = true)
}

object ComponentDescription {

  def masterComponent(name: String): ComponentDescription =
    ComponentDescription(name, Set(Master))

  def slaveComponent(name: String): ComponentDescription =
    ComponentDescription(name, Set(Slave))

  def allNodesComponent(name: String): ComponentDescription =
    ComponentDescription(name, Set(Master, Slave))
}
