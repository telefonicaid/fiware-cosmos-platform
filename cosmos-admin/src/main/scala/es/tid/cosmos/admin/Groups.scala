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

import es.tid.cosmos.api.profile._
import scala.util.Try

/** Admin commands for managing groups. */
private[admin] class Groups(dao: CosmosProfileDao) {
  import Groups._

  /** Create a group.
    *
    * @param name     the name of the group
    * @param minQuota the group's minimum, guaranteed quota
    * @return         true iff the group was successfully created
    */
  def create(name: String, minQuota: Int): Boolean =
    dao.withConnection { implicit c =>
      dao.registerGroup(GuaranteedGroup(name, Quota(minQuota)))
      true
    }

  /** List the existing groups.
    *
    * @return all the existing groups filtering out the implied
    *         [[es.tid.cosmos.api.profile.NoGroup]]
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
  def delete(name: String): Boolean = withGroup(name) { (c, _) =>
    dao.deleteGroup(name)(c)
  }

  /** Set an existing group's minimum, guaranteed quota.
    *
    * @param name  the name of the group
    * @param quota the new minimum quota
    * @return      true if the group was successfully updated with the new minimum quota
    */
  def setMinQuota(name: String, quota: Int): Boolean = withGroup(name) { (c, _) =>
    dao.setGroupQuota(name, Quota(quota))(c)
  }

  private def withGroup(name: String)
                       (action: (this.dao.type#Conn, String) => Unit): Boolean = {
    Try(dao.withTransaction { implicit c =>
      dao.getGroups.find(_.name == name) match {
        case None => {
          println(s"No group with name $name")
          false
        }
        case Some(group) => action(c, name)
      }
    }).isSuccess
  }
}

private object Groups {
  private def toUserFriendly(group: Group): String = s"${group.name} | ${group.minimumQuota}"
}
