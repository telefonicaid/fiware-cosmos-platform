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

import java.sql.{SQLException, Connection}

import anorm._
import anorm.SqlParser._
import play.Logger

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.UserState.UserState
import es.tid.cosmos.api.profile.dao.{CosmosDaoException, ProfileDao}
import es.tid.cosmos.api.quota._

private[sql] object PlayDbProfileDao extends ProfileDao[Connection] {

  private val AllUserFields = """u.cosmos_id, u.state, u.handle, u.email, u.machine_quota,
                                | u.api_key, u.api_secret, u.group_name""".stripMargin

  override def register(userId: UserId, reg: Registration, state: UserState)
                       (implicit c: Connection): CosmosProfile = {
    val apiCredentials = ApiCredentials.random()
    val defaultKey = NamedKey("default", reg.publicKey)
    val unpersistedProfile = CosmosProfile(
      id = -1,
      state = state,
      handle = reg.handle,
      email = reg.email,
      apiCredentials = apiCredentials,
      keys = Seq(defaultKey)
    )
    require(
      unpersistedProfile.capabilities.capabilities.isEmpty,
      "No support for user registration with capabilities already added.")
    val profileId = SQL(
      """INSERT INTO user(auth_realm, auth_id, handle, state, email, api_key, api_secret, group_name,
        |                 machine_quota)
        | VALUES ({auth_realm}, {auth_id}, {handle}, {state}, {email}, {api_key}, {api_secret},
        |         {group_name}, {machine_quota})"""
        .stripMargin)
      .on(
        "auth_realm" -> userId.realm,
        "auth_id" -> userId.id,
        "handle" -> reg.handle,
        "state" -> state.toString,
        "email" -> reg.email,
        "api_key" -> apiCredentials.apiKey,
        "api_secret" -> apiCredentials.apiSecret,
        "group_name" -> dbGroupName(unpersistedProfile.group),
        "machine_quota" -> unpersistedProfile.quota.toOptInt
      ).executeInsert(scalar[ProfileId].single)
    addPublicKey(profileId, defaultKey)
    val persistedProfile = unpersistedProfile.copy(id = profileId)
    persistedProfile
  }

  override def list()(implicit c: Connection): Seq[CosmosProfile] =
    lookup(SQL(s"""SELECT $AllUserFields, p.name, p.signature
                  |FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                  |ORDER BY cosmos_id""".stripMargin))

  override def lookupByProfileId(id: ProfileId)(implicit c: Connection): Option[CosmosProfile] =
    lookup(SQL(s"""SELECT $AllUserFields, p.name, p.signature
        |FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
        |WHERE u.cosmos_id = {id}""".stripMargin)
      .on("id" -> id)
    ).headOption

  override def lookupByUserId(userId: UserId)(implicit c: Connection): Option[CosmosProfile] =
    lookup(SQL(s"""SELECT $AllUserFields, p.name, p.signature
                  |FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                  |WHERE u.auth_realm = {realm} AND u.auth_id = {id}""".stripMargin)
      .on("realm" -> userId.realm, "id" -> userId.id)).headOption

  override def lookupByApiCredentials(creds: ApiCredentials)
                                     (implicit c: Connection): Option[CosmosProfile] =
    lookup(SQL(s"""SELECT $AllUserFields, p.name, p.signature
                  |FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                  |WHERE u.api_key = {key} AND u.api_secret = {secret}""".stripMargin)
      .on("key" -> creds.apiKey, "secret" -> creds.apiSecret)).headOption

  override def lookupByGroup(group: Group)(implicit c: Connection): Set[CosmosProfile] =
    lookup(SQL(s"""SELECT $AllUserFields, p.name, p.signature
                       | FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                       | WHERE u.group_name ${equalsOp(dbGroupName(group))} {groupName}"""
      .stripMargin).on("groupName" -> dbGroupName(group))).toSet

  override def lookupByHandle(handle: String)(implicit c: Connection): Option[CosmosProfile] =
    lookup(SQL(s"""SELECT $AllUserFields, p.name, p.signature
                  |FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                  |WHERE u.handle = {handle}""".stripMargin)
      .on("handle" -> handle)).headOption

  override def handleExists(handle: String)(implicit c: Connection): Boolean = {
    val usersWithThatHandle = SQL("SELECT count(*) from user WHERE handle = {handle}")
      .on("handle" -> handle)
      .as(scalar[ProfileId].single)
    usersWithThatHandle > 0
  }

  override def setHandle(id: ProfileId, handle: String)(implicit c: Connection): Unit =
    try {
      updateProfileField("handle", id, handle)
    } catch {
      case ex: SQLException =>
        Logger.error("Cannot update handle", ex)
        throw CosmosDaoException.duplicatedHandle(handle)
    }

  override def setEmail(id: ProfileId, email: String)(implicit c: Connection): Unit =
    updateProfileField("email", id, email)

  override def setPublicKeys(id: ProfileId, publicKeys: Seq[NamedKey])
                            (implicit c: Connection): Unit =
    try {
      SQL("DELETE FROM public_key WHERE cosmos_id = {id}").on("id" -> id).executeUpdate()
      publicKeys.foreach(key => addPublicKey(id, key))
    } catch {
      case ex: SQLException =>
        Logger.error(s"Cannot change $id keys", ex)
        throw CosmosDaoException.unknownUser(id)
    }

  override def setUserState(id: ProfileId, state: UserState.UserState)
                           (implicit c: Connection): Unit =
    updateProfileField("state", id, state.toString)

  override def setGroup(id: ProfileId, groupName: Option[String])(implicit c: Connection): Unit =
    updateProfileField("group_name", id, groupName)

  override def setMachineQuota(id: ProfileId, quota: Quota)(implicit c: Connection): Unit = {
    val updatedRows =
      SQL("UPDATE user SET machine_quota = {machine_quota} WHERE cosmos_id = {cosmos_id}")
        .on("cosmos_id" -> id, "machine_quota" -> quota.toOptInt)
        .executeUpdate()
    if (updatedRows == 0) throw CosmosDaoException.unknownUser(id)
  }

  /** Lookup Cosmos profiles retrieved by a custom query.
    *
    * @param query Query with the following output columns: cosmos id, handle, apiKey, apiSecret,
    *              name and signature
    * @return      Retrieved profiles
    */
  private def lookup(query: SimpleSql[Row])(implicit c: Connection): Seq[CosmosProfile] = {
    val rows = query().toList
    rows.map {
      case Row(
        id: Int,
        UserState(state),
        handle: String,
        email: String,
        machineQuota: Option[_],
        apiKey: String,
        apiSecret: String,
        groupName: Option[_],
        _, _
      ) =>
        val namedKeys = for {
          row <- rows if row[Int]("cosmos_id") == id
        } yield NamedKey(row[String]("name"), row[String]("signature"))
        CosmosProfile(
          id, state, handle, email,
          group = getGroup(groupName.asInstanceOf[Option[String]]),
          quota = Quota(machineQuota.asInstanceOf[Option[Int]]),
          capabilities = PlayDbCapabilityDao.userCapabilities(id),
          apiCredentials = ApiCredentials(apiKey, apiSecret),
          keys = namedKeys
        )
    }
  }

  private def updateProfileField(fieldName: String, id: ProfileId, value: Any)
                                (implicit c: Connection): Unit = {
    val updatedRows = SQL(s"UPDATE user SET $fieldName = {value} WHERE cosmos_id = {id}")
      .on("value" -> value, "id" -> id)
      .executeUpdate()
    if (updatedRows == 0) throw CosmosDaoException.unknownUser(id)
  }

  private def addPublicKey(id: ProfileId, publicKey: NamedKey)(implicit c: Connection) =
    SQL("""INSERT INTO public_key(cosmos_id, name, signature)
          | VALUES ({cosmos_id}, {name}, {signature})""".stripMargin)
      .on(
        "cosmos_id" -> id,
        "name" -> publicKey.name,
        "signature" -> publicKey.signature
      )
      .executeInsert()

  private def getGroup(maybeName: Option[String])(implicit c: Connection): Group =
    maybeName.flatMap(PlayDbGroupDao.lookupByName).getOrElse(NoGroup)

  /** Utility method for changing the operator in SQL statements when using optional values
    * that convert None to NULL in SQL.
    * <br/>
    * e.g
    *
    * {{{select * from user where group_name IS NULL}}}
    *
    * @param maybe the optional value. In case of None the operator will be 'IS', '=' otherwise
    * @return the operator to be used
    */
  private def equalsOp(maybe: Option[_]): String = maybe match {
    case None => "IS"
    case _ => "="
  }

  private def dbGroupName(group: Group): Option[String] = group match {
    case NoGroup => None
    case _ => Some(group.name)
  }
}
