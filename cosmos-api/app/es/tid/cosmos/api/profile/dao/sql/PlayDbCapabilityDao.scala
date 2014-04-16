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

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.Capability._
import es.tid.cosmos.api.profile.dao.CapabilityDao

private[sql] object PlayDbCapabilityDao extends CapabilityDao[Connection] {

  override def enable(id: ProfileId, capability: Capability)(implicit c: Connection): Unit =
    if (!PlayDbCapabilityDao.userCapabilities(id).hasCapability(capability)) {
      SQL("INSERT INTO user_capability(name, cosmos_id) VALUES ({name}, {cosmos_id})")
        .on("name" -> capability.toString, "cosmos_id" -> id)
        .executeInsert()
    }

  override def disable(id: ProfileId, capability: Capability)(implicit c: Connection): Unit =
    if (PlayDbCapabilityDao.userCapabilities(id).hasCapability(capability)) {
      SQL("DELETE FROM user_capability WHERE name = {name} AND cosmos_id = {cosmos_id}")
        .on("name" -> capability.toString, "cosmos_id" -> id)
        .executeInsert()
    }

  override def userCapabilities(id: ProfileId)(implicit c: Connection): UserCapabilities =
    SQL("SELECT name FROM user_capability WHERE cosmos_id = {cosmos_id}")
      .on("cosmos_id" -> id)
      .apply().collect { case Row(name: String) => name }
      .foldLeft(
        UntrustedUserCapabilities: UserCapabilities
      )((cap, name) => cap + Capability.withName(name))
}
