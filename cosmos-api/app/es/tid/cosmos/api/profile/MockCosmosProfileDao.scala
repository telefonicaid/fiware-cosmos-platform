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

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.profile.UserState._
import es.tid.cosmos.api.profile.MockCosmosProfileDao._
import es.tid.cosmos.api.quota._
import es.tid.cosmos.servicemanager.clusters.ClusterId

trait MockCosmosProfileDaoComponent extends CosmosProfileDaoComponent {
  def cosmosProfileDao: CosmosProfileDao = new MockCosmosProfileDao
}

/**
 * Mock to be used in tests for handling the profile DAO. Thread-safe.
 */
class MockCosmosProfileDao extends CosmosProfileDao {

  type Conn = DummyConnection.type

  @volatile private var users = Map[UserId, CosmosProfile]()
  @volatile private var clusters = List[ClusterAssignment]()
  @volatile private var groups: Set[Group] = Set(NoGroup)

  def withConnection[A](block: (Conn) => A): A = block(DummyConnection)
  def withTransaction[A](block: (Conn) => A): A = block(DummyConnection)

  override def registerUser(userId: UserId, reg: Registration, state: UserState)
                           (implicit c: Conn): CosmosProfile = {
    val cosmosProfile = CosmosProfile(
      id = users.size,
      state = state,
      handle = reg.handle,
      email = reg.email,
      apiCredentials = ApiCredentials.random(),
      keys = List(NamedKey("default", reg.publicKey))
    )
    require(!users.values.exists(_.handle == reg.handle), s"Duplicated handle: ${reg.handle}")
    require(groups.contains(cosmosProfile.group), s"Group not registered: ${cosmosProfile.group}")
    users.synchronized { users = users.updated(userId, cosmosProfile) }
    cosmosProfile
  }

  override def getAllUsers()(implicit c: Conn): Seq[CosmosProfile] = users.values.toSeq

  override def getProfileId(userId: UserId)(implicit c: Conn): Option[ProfileId] =
    users.get(userId).map(_.id)

  override def getMachineQuota(id: ProfileId)(implicit c: Conn): Quota =
    users.collectFirst {
      case (_, profile) if profile.id == id => profile.quota
    }.getOrElse(EmptyQuota)

  override def setMachineQuota(id: ProfileId, quota: Quota)(implicit c: Conn) {
    updateProfile(id) { profile => profile.copy(quota = quota) }
  }

  override def enableUserCapability(id: ProfileId, capability: Capability.Value)
                                   (implicit c: Conn) {
    updateProfile(id) { profile =>
      profile.copy(capabilities = profile.capabilities + capability)
    }
  }

  override def disableUserCapability(id: ProfileId, capability: Capability.Value)
                                    (implicit c: Conn) {
    updateProfile(id) { profile =>
      profile.copy(capabilities = profile.capabilities - capability)
    }
  }

  override def handleExists(handle: String)(implicit c: Conn): Boolean =
    users.values.exists(_.handle == handle)

  override def lookupByProfileId(id: ProfileId)(implicit c: Conn): Option[CosmosProfile] =
    users.values.find(_.id == id)

  override def lookupByUserId(userId: UserId)(implicit c: Conn): Option[CosmosProfile] =
    users.get(userId)

  override def lookupByApiCredentials(creds: ApiCredentials)
                                     (implicit c: Conn): Option[CosmosProfile] =
    users.collectFirst {
      case (_, profile) if profile.apiCredentials == creds => profile
    }

  override def assignCluster(assignment: ClusterAssignment)(implicit c: Conn) {
    clusters.synchronized {
      require(!clusters.exists(_.clusterId == assignment.clusterId), "Cluster already assigned")
      clusters = clusters :+ assignment
    }
  }

  override def ownerOf(clusterId: ClusterId)(implicit c: Conn): Option[ProfileId] =
    clusters.find(_.clusterId == clusterId).map(_.ownerId)

  override def clustersOf(id: ProfileId)(implicit c: Conn): Seq[ClusterAssignment] =
    clusters.filter(_.ownerId == id)

  override def setHandle(id: ProfileId, handle: String)(implicit c: Conn) {
    updateProfile(id) { profile =>
      if (profile.handle != handle && users.values.exists(_.handle == handle))
        throw CosmosProfileException.duplicatedHandle(handle)
      profile.copy(handle = handle)
    }
  }

  override def setEmail(id: ProfileId, email: String)(implicit c: Conn) {
    updateProfile(id) { profile =>
      profile.copy(email = email)
    }
  }

  override def setUserState(id: ProfileId, userState: UserState)(implicit c: Conn) {
    updateProfile(id) { profile =>
      profile.copy(state = userState)
    }
  }

  override def setPublicKeys(id: ProfileId, publicKeys: Seq[NamedKey])(implicit c: Conn) {
    updateProfile(id) { profile =>
      profile.copy(keys = publicKeys)
    }
  }

  override def lookupByGroup(group: Group)(implicit c: Conn): Set[CosmosProfile] =
    users.values.filter(profile => profile.group == group).toSet

  override def getGroups(implicit c: Conn): Set[Group] = groups

  override def registerGroup(group: Group)(implicit c: Conn) {
    groups.synchronized { groups += group }
  }

  override def deleteGroup(name: String)(implicit c: Conn) {
    val groupUsers = users.values.filter(profile => profile.group.name == name)
    groupUsers.foreach { profile =>
      updateProfile(profile.id) { profile => profile.copy(group = NoGroup) }
    }
    groups.synchronized { groups = groups.filterNot(_.name == name) }
  }

  override def lookupByHandle(handle: String)(implicit c: Conn): Option[CosmosProfile] =
    users.values.find(_.handle == handle)

  override def setGroup(id: ProfileId, groupName: Option[String])(implicit c: Conn) {
    val maybeGroup = if (groupName.isEmpty) Some(NoGroup) else groupName.flatMap(groupByName)
    for (group <- maybeGroup) yield {
      updateProfile(id)(profile => profile.copy(group = group))
    }
  }

  override def setGroupQuota(name: String, minQuota: LimitedQuota)(implicit c: Conn) {
    val updated = GuaranteedGroup(name, minQuota)
    updateUsersWithGroup(updated)
    groups.synchronized { groups = groups.filterNot(_.name == name) + updated }
  }

  private def updateProfile(id: ProfileId)(f: CosmosProfile => CosmosProfile) {
    users.synchronized {
      val maybeId = users.collectFirst {
        case (userId, profile) if profile.id == id => userId
      }
      maybeId match {
        case Some(userId) => users = users.updated(userId, f(users(userId)))
        case None => throw CosmosProfileException.unknownUser(id)
      }
    }
  }

  private def groupByName(name: String): Option[Group] = {
    require(groups.exists(_.name == name), s"Unregistered group: $name")
    groups.find(_.name == name)
  }

  private def updateUsersWithGroup(group: Group) {
    for {
      userId <- users.keys if users(userId).group.name == group.name
      profile = users(userId)
    } yield users.synchronized {
      users = users.updated(userId, profile.copy(group = group))
    }
  }
}

object MockCosmosProfileDao {
  object DummyConnection
}
