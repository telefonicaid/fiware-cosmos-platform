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

package es.tid.cosmos.admin


import es.tid.cosmos.admin.Util._
import es.tid.cosmos.admin.validation.GroupChecks
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.quota.{GuaranteedGroup, Group, NoGroup, Quota}

/** Admin commands for managing groups. */
private[admin] class Groups(override val dao: CosmosProfileDao) extends GroupChecks {
  import Groups._

  /** Create a group.
    *
    * @param name     the name of the group
    * @param minQuota the group's minimum, guaranteed quota
    * @return         true iff the group was successfully created
    */
  def create(name: String, minQuota: Int): Boolean = dao.withTransaction { implicit c =>
    tryAction { Some(dao.registerGroup(GuaranteedGroup(name, Quota(minQuota)))) }
  }

  /** List the existing groups.
    *
    * @return all the existing groups filtering out the implied
    *         [[es.tid.cosmos.api.quota.NoGroup]]
    */
  def list: String = dao.withConnection { implicit c =>
    val groups = dao.getGroups - NoGroup
    if (groups.isEmpty) "No groups available"
    else s"Available groups: [Name | Minimum Quota]:\n${groups.map(toUserFriendly).mkString("\n")}"
  }

  /** Delete an existing group.
    *
    * @param name the group's name
    * @return     true iff the group was successfully deleted
    */
  def delete(name: String): Boolean = dao.withTransaction { implicit c =>
    tryAction { for (group <- withGroup(name)) yield dao.deleteGroup(group.name) }
  }

  /** Set an existing group's minimum, guaranteed quota.
    *
    * @param name  the name of the group
    * @param quota the new minimum quota
    * @return      true if the group was successfully updated with the new minimum quota
    */
  def setMinQuota(name: String, quota: Int): Boolean = dao.withTransaction { implicit c =>
    tryAction { for (group <- withGroup(name)) yield dao.setGroupQuota(group.name, Quota(quota)) }
  }
}

private object Groups {
  private def toUserFriendly(group: Group): String = s"${group.name} | ${group.minimumQuota}"
}
