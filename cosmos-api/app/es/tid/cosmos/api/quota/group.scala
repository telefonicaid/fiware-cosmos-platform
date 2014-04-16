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
