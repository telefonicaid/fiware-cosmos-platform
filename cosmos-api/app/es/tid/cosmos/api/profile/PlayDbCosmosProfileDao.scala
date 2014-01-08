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

import java.sql.{SQLException, Connection}
import java.util.Date

import anorm._
import anorm.SqlParser._
import play.Logger
import play.api.db.DB
import play.api.Play.current

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.profile.UserState._
import es.tid.cosmos.api.profile.Capability._
import es.tid.cosmos.api.quota._
import es.tid.cosmos.servicemanager.clusters.ClusterId

trait PlayDbCosmosProfileDaoComponent extends CosmosProfileDaoComponent {
  lazy val cosmosProfileDao: CosmosProfileDao = new PlayDbCosmosProfileDao
}

class PlayDbCosmosProfileDao extends CosmosProfileDao {
  import PlayDbCosmosProfileDao._

  type Conn = Connection

  def withConnection[A](block: (Conn) => A): A = DB.withConnection[A](block)
  def withTransaction[A](block: (Conn) => A): A = DB.withTransaction[A](block)

  override def registerUser(userId: UserId, reg: Registration, state: UserState)
                           (implicit c: Conn): CosmosProfile = {
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
        "machine_quota" -> unpersistedProfile.quota.toInt
      ).executeInsert(scalar[ProfileId].single)
    addPublicKey(profileId, defaultKey)
    val persistedProfile = unpersistedProfile.copy(id = profileId)
    persistedProfile
  }

  override def getAllUsers()(implicit c: Conn): Seq[CosmosProfile] =
    lookup(SQL(s"""SELECT $AllUserFields, p.name, p.signature
                  |FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                  |ORDER BY cosmos_id""".stripMargin))

  override def getProfileId(userId: UserId)(implicit c: Conn): Option[ProfileId] =
    SQL("SELECT cosmos_id FROM user WHERE auth_realm = {realm} AND auth_id = {id}")
      .on("realm" -> userId.realm, "id" -> userId.id)
      .as(scalar[ProfileId].singleOpt)

  override def getMachineQuota(id: ProfileId)(implicit c: Conn): Quota =
    SQL("SELECT machine_quota FROM user WHERE cosmos_id = {cosmos_id}")
      .on("cosmos_id" -> id)
      .apply()
      .collectFirst {
        case Row(machineQuota: Option[_]) => Quota(machineQuota.asInstanceOf[Option[Int]])
      }
      .getOrElse(EmptyQuota)

  override def setMachineQuota(id: ProfileId, quota: Quota)(implicit c: Conn) {
    val updatedRows =
      SQL("UPDATE user SET machine_quota = {machine_quota} WHERE cosmos_id = {cosmos_id}")
        .on("cosmos_id" -> id, "machine_quota" -> quota.toInt)
        .executeUpdate()
    if (updatedRows == 0) throw CosmosProfileException.unknownUser(id)
  }

  override def enableUserCapability(id: ProfileId, capability: Capability.Value)(implicit c: Conn) {
    if (!getUserCapabilities(id).hasCapability(capability)) {
      SQL("INSERT INTO user_capability(name, cosmos_id) VALUES ({name}, {cosmos_id})")
        .on("name" -> capability.toString, "cosmos_id" -> id)
        .executeInsert()
    }
  }

  override def disableUserCapability(id: ProfileId, capability: Capability)(implicit c: Conn) {
    if (getUserCapabilities(id).hasCapability(capability)) {
      SQL("DELETE FROM user_capability WHERE name = {name} AND cosmos_id = {cosmos_id}")
        .on("name" -> capability.toString, "cosmos_id" -> id)
        .executeInsert()
    }
  }

  override def handleExists(handle: String)(implicit c: Conn): Boolean = {
    val usersWithThatHandle = SQL("SELECT count(*) from user WHERE handle = {handle}")
      .on("handle" -> handle)
      .as(scalar[ProfileId].single)
    usersWithThatHandle > 0
  }

  override def lookupByProfileId(id: ProfileId)(implicit c: Conn): Option[CosmosProfile] =
    lookup(SQL(s"SELECT $AllUserFields, p.name, p.signature FROM user u WHERE u.id = {id}").
      on("id" -> id)
    ).headOption

  override def lookupByUserId(userId: UserId)(implicit c: Conn): Option[CosmosProfile] =
    lookup(SQL(s"""SELECT $AllUserFields, p.name, p.signature
                  |FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                  |WHERE u.auth_realm = {realm} AND u.auth_id = {id}""".stripMargin)
      .on("realm" -> userId.realm, "id" -> userId.id)).headOption

  override def lookupByApiCredentials(creds: ApiCredentials)
                                     (implicit c: Conn): Option[CosmosProfile] =
    lookup(SQL(s"""SELECT $AllUserFields, p.name, p.signature
                  |FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                  |WHERE u.api_key = {key} AND u.api_secret = {secret}""".stripMargin)
      .on("key" -> creds.apiKey, "secret" -> creds.apiSecret)).headOption

  override def lookupByGroup(group: Group)(implicit c: Conn): Set[CosmosProfile] =
    lookup(SQL(s"""SELECT $AllUserFields, p.name, p.signature
                       | FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                       | WHERE u.group_name ${equalsOp(dbGroupName(group))} {groupName}"""
      .stripMargin).on("groupName" -> dbGroupName(group))).toSet

  override def lookupByHandle(handle: String)(implicit c: Conn): Option[CosmosProfile] =
    lookup(SQL(s"""SELECT $AllUserFields, p.name, p.signature
                  |FROM user u LEFT OUTER JOIN public_key p ON (u.cosmos_id = p.cosmos_id)
                  |WHERE u.handle = {handle}""".stripMargin)
      .on("handle" -> handle)).headOption

  override def getGroups(implicit c: Conn): Set[Group] = {
    val registeredGroups = SQL("SELECT name, min_quota FROM user_group").apply().collect(ToGroup)
    Set[Group](NoGroup) ++ registeredGroups
  }

  private def getGroup(maybeName: Option[String])(implicit c: Conn): Group = {
    val maybeGroup = maybeName.flatMap(name =>
      SQL("""SELECT name, min_quota FROM user_group WHERE name = {name}""".stripMargin)
        .on("name" -> name).apply().collectFirst(ToGroup))

    maybeGroup.getOrElse(NoGroup)
  }

  override def registerGroup(group: Group)(implicit c: Conn) = {
    SQL("""INSERT INTO user_group(name, min_quota)
          | VALUES ({name}, {min_quota})""".stripMargin).on(
      "name" -> group.name,
      "min_quota" -> group.minimumQuota.toInt
    ).execute()
  }

  override def setGroup(id: ProfileId, groupName: Option[String])(implicit c: Conn) {
    updateProfileField("group_name", id, groupName)
  }

  override def setGroupQuota(name: String, minQuota: LimitedQuota)(implicit c: Conn) {
    SQL("UPDATE user_group SET min_quota = {min_quota} where name = {name}")
      .on("min_quota" -> minQuota.toInt, "name" -> name).executeUpdate()
  }

  /** ''Note:'' Referential integrity is assumed to be delegated to the DB so as to set the group's
    * users to have NoGroup/NULL
    *
    * @see CosmosProfileDao
    */
  override def deleteGroup(name: String)(implicit c: Conn) {
    SQL("DELETE from user_group where name = {name}").on("name" -> name).execute()
  }

  override def assignCluster(assignment: ClusterAssignment)(implicit c: Conn) {
    SQL("""INSERT INTO cluster(cluster_id, owner, creation_date)
          | VALUES ({cluster_id}, {owner}, {creation_date})""".stripMargin).on(
      "cluster_id" -> assignment.clusterId.toString,
      "owner" -> assignment.ownerId,
      "creation_date" -> assignment.creationDate
    ).execute()
  }

  override def ownerOf(clusterId: ClusterId)(implicit c: Conn): Option[ProfileId] =
    SQL("SELECT owner FROM cluster WHERE cluster_id = {cluster_id}")
      .on("cluster_id" -> clusterId.id)
      .as(scalar[ProfileId].singleOpt)

  override def clustersOf(id: ProfileId)(implicit c: Conn): Seq[ClusterAssignment] =
    SQL("SELECT cluster_id, creation_date FROM cluster WHERE owner = {owner}")
      .on("owner" -> id)
      .apply() collect {
        case Row(clusterId: String, creationDate: Date) =>
          ClusterAssignment(ClusterId(clusterId), id, creationDate)
      }

  /** Lookup Cosmos profiles retrieved by a custom query.
    *
    * @param query Query with the following output columns: cosmos id, handle, apiKey, apiSecret,
    *              name and signature
    * @return      Retrieved profiles
    */
  private def lookup(query: SimpleSql[Row])(implicit c: Conn): Seq[CosmosProfile] = {
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
          _, _) => {
        val namedKeys = for {
          row <- rows if row[Int]("cosmos_id") == id
        } yield NamedKey(row[String]("name"), row[String]("signature"))
        CosmosProfile(
          id, state, handle, email,
          group = getGroup(groupName.asInstanceOf[Option[String]]),
          quota = Quota(machineQuota.asInstanceOf[Option[Int]]),
          capabilities = getUserCapabilities(id),
          apiCredentials = ApiCredentials(apiKey, apiSecret),
          keys = namedKeys
        )
      }
    }
  }

  override def setHandle(id: ProfileId, handle: String)(implicit c: Conn) {
    try {
      updateProfileField("handle", id, handle)
    } catch {
      case ex: SQLException => {
        Logger.error("Cannot update handle", ex)
        throw CosmosProfileException.duplicatedHandle(handle)
      }
    }
  }

  override def setEmail(id: ProfileId, email: String)(implicit c: Conn) {
    updateProfileField("email", id, email)
  }

  override def setUserState(id: ProfileId, state: UserState.UserState)(implicit c: Conn) {
    updateProfileField("state", id, state.toString)
  }

  override def setPublicKeys(id: ProfileId, publicKeys: Seq[NamedKey])(implicit c: Conn) {
    try {
      SQL("DELETE FROM public_key WHERE cosmos_id = {id}").on("id" -> id).executeUpdate()
      publicKeys.foreach(key => addPublicKey(id, key))
    } catch {
      case ex: SQLException => {
        Logger.error(s"Cannot change $id keys", ex)
        throw CosmosProfileException.unknownUser(id)
      }
    }
  }

  private def updateProfileField(fieldName: String, id: ProfileId, value: Any)(implicit c: Conn) {
    val updatedRows = SQL(s"UPDATE user SET $fieldName = {value} WHERE cosmos_id = {id}")
        .on("value" -> value, "id" -> id)
        .executeUpdate()
    if (updatedRows == 0) throw CosmosProfileException.unknownUser(id)
  }

  private def addPublicKey(id: ProfileId, publicKey: NamedKey)(implicit c: Conn) =
    SQL("""INSERT INTO public_key(cosmos_id, name, signature)
         | VALUES ({cosmos_id}, {name}, {signature})""".stripMargin)
    .on(
      "cosmos_id" -> id,
      "name" -> publicKey.name,
      "signature" -> publicKey.signature
    )
    .executeInsert()

  private def getUserCapabilities(id: ProfileId)(implicit c: Conn): UserCapabilities =
    SQL("SELECT name FROM user_capability WHERE cosmos_id = {cosmos_id}")
      .on("cosmos_id" -> id)
      .apply().collect { case Row(name: String) => name }
      .foldLeft(
      UntrustedUserCapabilities: UserCapabilities
    )((cap, name) => cap + Capability.withName(name))
}

object PlayDbCosmosProfileDao {
  private val AllUserFields = """u.cosmos_id, u.state, u.handle, u.email, u.machine_quota,
                                 | u.api_key, u.api_secret, u.group_name""".stripMargin

  private val ToGroup: PartialFunction[Row, Group] = {
    case Row(name: String, minimumQuota: Int) =>
      GuaranteedGroup(name, Quota(minimumQuota))
  }

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
