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

import java.util.UUID

case class ClusterId(val id: String) {
  require(
    id.forall(Character.isLetterOrDigit),
    s"Cluster IDs can only contain alphanumeric characters. Provided id: %id")

  def this(uuid: UUID = UUID.randomUUID()) = this(uuid.toString.replace("-", ""))
  override def toString: String = id
}

object ClusterId {
  /**
   * Creates a cluster ID using UUID.randomUUID
   * @return the ID
   */
  def apply() = new ClusterId(UUID.randomUUID())

  def apply(uuid: UUID) = new ClusterId(uuid)
}
