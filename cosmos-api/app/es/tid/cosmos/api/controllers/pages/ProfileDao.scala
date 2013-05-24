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

package es.tid.cosmos.api.controllers.pages

import java.sql.Connection

import anorm._
import anorm.SqlParser._

object ProfileDao {
  def registerUserInDatabase(userId: String, reg: Registration)(implicit c: Connection): Long = {
    val cosmosId = SQL("INSERT INTO user(user_id, handle) VALUES ({user_id}, {handle})")
      .on("user_id" -> userId, "handle" -> reg.handle)
      .executeInsert(scalar[Long].single)
    SQL("""INSERT INTO public_key(cosmos_id, name, signature)
           VALUES ({cosmos_id}, 'default', {signature})""")
      .on("cosmos_id" -> cosmosId, "signature" -> reg.publicKey)
      .executeInsert()
    cosmosId
  }

  def getCosmosId(userId: String)(implicit c: Connection): Option[Long] =
    SQL("SELECT cosmos_id FROM user WHERE user_id = {user_id}")
      .on("user_id" -> userId)
      .as(scalar[Long].singleOpt)

  def lookupByUserId(userId: String)(implicit c: Connection): Option[CosmosProfile] = {
    val rows = SQL("""SELECT u.cosmos_id, u.handle, p.name, p.signature
                     | FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                     | WHERE u.user_id = {user_id}""".stripMargin)
      .on("user_id" -> userId)
      .apply()
      .toList
    rows.headOption.map {
      case Row(id: Int, handle: String, _, _) => {
        val namedKeys = rows.collect {
          case Row(_, _, name: String, signature: String) => NamedKey(name, signature)
        }
        CosmosProfile(id, handle, namedKeys: _*)
      }
    }
  }
}
