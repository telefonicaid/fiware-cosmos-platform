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

package es.tid.cosmos.api.profile.dao.sql

import java.sql.Connection

import anorm._

import es.tid.cosmos.api.profile.dao.GroupDao
import es.tid.cosmos.api.quota._

private[sql] object PlayDbGroupDao extends GroupDao[Connection] {

  override def lookupByName(name: String)(implicit c: Connection): Option[GuaranteedGroup] =
    SQL("SELECT name, min_quota FROM user_group WHERE name = {name}")
      .on("name" -> name)
      .apply()
      .collectFirst(PlayDbGroupDao.ToGroup)

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
      .collect(ToGroup)
      .force
    Set[Group](NoGroup) ++ registeredGroups
  }

  override def setQuota(name: String, minQuota: LimitedQuota)(implicit c: Connection) {
    SQL("UPDATE user_group SET min_quota = {min_quota} where name = {name}")
      .on("min_quota" -> minQuota.toOptInt, "name" -> name).executeUpdate()
  }

  val ToGroup: PartialFunction[Row, GuaranteedGroup] = {
    case Row(name: String, minimumQuota: Int) =>
      GuaranteedGroup(name, Quota(minimumQuota))
  }
}
