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

package es.tid.cosmos.api.profile.sql

import java.sql.Connection

import anorm._

import es.tid.cosmos.api.profile.GroupDao
import es.tid.cosmos.api.quota.{LimitedQuota, NoGroup, Group}

private[sql] object PlayDbGroupDao extends GroupDao[Connection] {

  override def register(group: Group)(implicit c: Connection) = {
    SQL("""INSERT INTO user_group(name, min_quota)
          | VALUES ({name}, {min_quota})""".stripMargin).on(
        "name" -> group.name,
        "min_quota" -> group.minimumQuota.toOptInt
      ).execute()
  }

  /** ''Note:'' Referential integrity is assumed to be delegated to the DB so as to set the group's
    * users to have NoGroup/NULL
    *
    * @see CosmosProfileDao
    */
  override def delete(name: String)(implicit c: Connection) {
    SQL("DELETE from user_group where name = {name}").on("name" -> name).execute()
  }

  override def list()(implicit c: Connection): Set[Group] = {
    val registeredGroups = SQL("SELECT name, min_quota FROM user_group")
      .apply()
      .collect(PlayDbProfileDao.ToGroup)
      .force
    Set[Group](NoGroup) ++ registeredGroups
  }

  override def setQuota(name: String, minQuota: LimitedQuota)(implicit c: Connection) {
    SQL("UPDATE user_group SET min_quota = {min_quota} where name = {name}")
      .on("min_quota" -> minQuota.toOptInt, "name" -> name).executeUpdate()
  }
}
