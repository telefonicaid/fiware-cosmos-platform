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

package es.tid.cosmos.servicemanager

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
