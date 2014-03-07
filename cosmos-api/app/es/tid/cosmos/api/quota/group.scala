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

package es.tid.cosmos.api.quota

/**
 * Representation of a user group.
 */
sealed trait Group {
  val name: String

  /**
   * The minimum machine quota that should be guaranteed for this group.
   *
   * @return the quota of machines guaranteed to always be available within the group
   */
  def minimumQuota: LimitedQuota
}

/**
 * A user group with a minimum number of machines guaranteed to be available for usage.
 *
 * @constructor create a new group
 * @param name the group's name
 * @param minimumQuota the minimum machine quota that should always be available for the group.
 *                     If [[es.tid.cosmos.api.quota.EmptyQuota]] the group offers no minimum
 *                     quota guarantees.
 * @throws IllegalArgumentException if the minimum quota is
 *                                  [[es.tid.cosmos.api.quota.UnlimitedQuota]] because an unbound
 *                                  quota cannot be guaranteed.
 */
case class GuaranteedGroup(
  override val name: String,
  override val minimumQuota: LimitedQuota) extends Group

/**
 * Representation of a null group object for users that do not belong to any specific group.
 * It does not offer any quota guarantees.
 */
case object NoGroup extends Group {
  override val name = "No Group"
  override val minimumQuota = EmptyQuota
}

object Group {
  /** Alias of NoGroup with Group type to avoid the pitfalls of the NoGroup.type inference */
  val noGroup: Group = NoGroup
}
