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

package es.tid.cosmos.api.profile

import java.sql.Connection

import anorm._
import anorm.SqlParser._
import play.api.db.DB
import play.api.Play.current

import es.tid.cosmos.api.authorization.ApiCredentials
import es.tid.cosmos.api.controllers.pages.{NamedKey, CosmosProfile, Registration}
import es.tid.cosmos.servicemanager.ClusterId

trait PlayDbCosmosProfileDaoComponent extends CosmosProfileDaoComponent {
  lazy val cosmosProfileDao: CosmosProfileDao = new PlayDbCosmosProfileDao
}

class PlayDbCosmosProfileDao extends CosmosProfileDao {
  type Conn = Connection

  def withConnection[A](block: (Conn) => A): A = DB.withTransaction[A](block)
  def withTransaction[A](block: (Conn) => A): A = DB.withTransaction[A](block)

  override def registerUserInDatabase(userId: UserId, reg: Registration)(implicit c: Conn): Long = {
    val credentials = ApiCredentials.random()
    val cosmosId = SQL(
      """INSERT INTO user(auth_realm, auth_id, handle, api_key, api_secret)
        | VALUES ({auth_realm}, {auth_id}, {handle}, {api_key}, {api_secret})""".stripMargin)
      .on(
        "auth_realm" -> userId.realm,
        "auth_id" -> userId.id,
        "handle" -> reg.handle,
        "api_key" -> credentials.apiKey,
        "api_secret" -> credentials.apiSecret
      ).executeInsert(scalar[Long].single)
    SQL("""INSERT INTO public_key(cosmos_id, name, signature)
           VALUES ({cosmos_id}, 'default', {signature})""")
      .on("cosmos_id" -> cosmosId, "signature" -> reg.publicKey)
      .executeInsert()
    cosmosId
  }

  override def getCosmosId(userId: UserId)(implicit c: Conn): Option[Long] =
    SQL("SELECT cosmos_id FROM user WHERE auth_realm = {realm} AND auth_id = {id}")
      .on("realm" -> userId.realm, "id" -> userId.id)
      .as(scalar[Long].singleOpt)

  override def getMachineQuota(cosmosId: Long)(implicit c: Conn): Quota =
    SQL("SELECT machine_quota FROM user WHERE cosmos_id = {cosmos_id}")
      .on("cosmos_id" -> cosmosId)
      .apply()
      .collectFirst {
        case Row(machineQuota: Option[Int]) => Quota(machineQuota)
      }
      .getOrElse(EmptyQuota)

  override def setMachineQuota(cosmosId: Long, quota: Quota)(implicit c: Conn): Boolean = {
    val quotaValue = quota match {
      case UnlimitedQuota => "NULL"
      case EmptyQuota => "0"
      case FiniteQuota(limit) => limit.toString
    }
    SQL("UPDATE user SET machine_quota = {machine_quota}")
      .on("machine_quota" -> quotaValue)
      .executeUpdate() > 0
  }

  override def handleExists(handle: String)(implicit c: Conn): Boolean = {
    val usersWithThatHandle = SQL("SELECT count(*) from user WHERE handle = {handle}")
      .on("handle" -> handle)
      .as(scalar[Long].single)
    usersWithThatHandle > 0
  }

  override def lookupByUserId(userId: UserId)(implicit c: Conn): Option[CosmosProfile] =
    lookup(SQL("""SELECT u.cosmos_id, u.handle, u.machine_quota, u.api_key, u.api_secret, p.name,
                 | p.signature
                 | FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                 | WHERE u.auth_realm = {realm} AND u.auth_id = {id}""".stripMargin)
      .on("realm" -> userId.realm, "id" -> userId.id))

  override def lookupByApiCredentials(creds: ApiCredentials)(implicit c: Conn): Option[CosmosProfile] =
    lookup(SQL("""SELECT u.cosmos_id, u.handle, u.machine_quota, u.api_key, u.api_secret, p.name,
                 | p.signature
                 | FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                 | WHERE u.api_key = {key} AND u.api_secret = {secret}""".stripMargin)
      .on("key" -> creds.apiKey, "secret" -> creds.apiSecret))

  override def assignCluster(clusterId: ClusterId, ownerId: Long)(implicit c: Conn) {
    SQL("INSERT INTO cluster(cluster_id, owner) VALUES ({cluster_id}, {owner})")
      .on("cluster_id" -> clusterId.toString, "owner" -> ownerId)
      .execute()
  }

  override def clustersOf(cosmosId: Long)(implicit c: Conn): Seq[ClusterId] =
    SQL("SELECT cluster_id FROM cluster WHERE owner = {owner}")
      .on("owner" -> cosmosId)
      .as(str("cluster_id").map(ClusterId.apply) *)

  /**
   * Lookup Cosmos profile by a custom query.
   *
   * @param query Query with the following output columns: cosmos id, handle, apiKey, apiSecret,
   *              name and signature
   * @return      A cosmos profile or nothing
   */
  private def lookup(query: SimpleSql[Row])(implicit c: Conn): Option[CosmosProfile] = {
    val rows = query().toList
    rows.headOption.map {
      case Row(id: Int, handle: String, machineQuota: Option[Int], apiKey: String,
          apiSecret: String, _, _) => {
        val namedKeys = rows.map(row => NamedKey(row[String]("name"), row[String]("signature")))
        CosmosProfile(
          id, handle, Quota(machineQuota), ApiCredentials(apiKey, apiSecret), namedKeys.toSeq)
      }
    }
  }
}
