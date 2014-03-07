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

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.Capability._
import es.tid.cosmos.api.profile.UserCapabilities

private[sql] object PlayDbCapabilityDao extends CapabilityDao[Connection] {

  override def enable(id: ProfileId, capability: Capability)(implicit c: Connection) {
    if (!PlayDbCapabilityDao.userCapabilities(id).hasCapability(capability)) {
      SQL("INSERT INTO user_capability(name, cosmos_id) VALUES ({name}, {cosmos_id})")
        .on("name" -> capability.toString, "cosmos_id" -> id)
        .executeInsert()
    }
  }

  override def disable(id: ProfileId, capability: Capability)(implicit c: Connection) {
    if (PlayDbCapabilityDao.userCapabilities(id).hasCapability(capability)) {
      SQL("DELETE FROM user_capability WHERE name = {name} AND cosmos_id = {cosmos_id}")
        .on("name" -> capability.toString, "cosmos_id" -> id)
        .executeInsert()
    }
  }

  override def userCapabilities(id: ProfileId)(implicit c: Connection): UserCapabilities =
    SQL("SELECT name FROM user_capability WHERE cosmos_id = {cosmos_id}")
      .on("cosmos_id" -> id)
      .apply().collect { case Row(name: String) => name }
      .foldLeft(
        UntrustedUserCapabilities: UserCapabilities
      )((cap, name) => cap + Capability.withName(name))
}
