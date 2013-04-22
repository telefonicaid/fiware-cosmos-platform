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

class ClusterId(val uuid: UUID = UUID.randomUUID()) {
  override def toString = uuid.toString
  override def equals(other: Any): Boolean = other match {
    case that: ClusterId => this.uuid.compareTo(that.uuid) == 0
    case _ => false
  }
  private lazy val cachedHashCode: Int = (17 +
    31 * uuid.getLeastSignificantBits.hashCode() +
    31 * uuid.getMostSignificantBits.hashCode())
  override def hashCode: Int = cachedHashCode
}

object ClusterId{
  /**
   * Creates a cluster ID using UUID.randomUUID
   * @return the ID
   */
  def apply() = new ClusterId(UUID.randomUUID())

  def apply(id: String) = new ClusterId(UUID.fromString(id))
}
