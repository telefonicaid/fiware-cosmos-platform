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
import es.tid.cosmos.api.profile.Capability._
import es.tid.cosmos.api.profile.UserState._

trait MockCosmosProfileDaoComponent extends CosmosProfileDaoComponent {
  def cosmosProfileDao: CosmosProfileDao = new MockCosmosProfileDao
}

/**
 * Mock to be used in tests for handling the profile DAO. Not thread-safe.
 */
class MockCosmosProfileDao extends CosmosProfileDao with DefaultUserProperties {

  type Conn = MockCosmosProfileDao.DummyConnection.type

  @volatile private var users: Map[UserId, CosmosProfile] = Map.empty
  @volatile private var clusters: List[ClusterAssignment] = List.empty
  @volatile private var groupsWithUsers: Map[Group, Set[UserId]] = Map(NoGroup -> Set.empty)

  def withConnection[A](block: (Conn) => A): A = block(MockCosmosProfileDao.DummyConnection)
  def withTransaction[A](block: (Conn) => A): A = block(MockCosmosProfileDao.DummyConnection)

  override def registerUser(userId: UserId, reg: Registration)(implicit c: Conn): CosmosProfile = {
    val credentials = ApiCredentials.random()
    require(!users.values.exists(_.handle == reg.handle), s"Duplicated handle: ${reg.handle}")
    require(groupsWithUsers.contains(defaultGroup), s"Group not registered: $defaultGroup")
    val cosmosProfile = CosmosProfile(
      id = users.size,
      state = Enabled,
      handle = reg.handle,
      email = reg.email,
      group = defaultGroup,
      quota = defaultQuota,
      capabilities = UntrustedUserCapabilities,
      apiCredentials = credentials,
      keys = List(NamedKey("default", reg.publicKey))
    )
    users.synchronized { users = users.updated(userId, cosmosProfile) }
    groupsWithUsers.synchronized {
      groupsWithUsers = groupsWithUsers.updated(defaultGroup, groupsWithUsers(defaultGroup) + userId)
    }
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
    users.synchronized {
      val userToUpdate = users.find(_._2.id == id)
      userToUpdate.foreach {
        case (userId, profile) => users = users.updated(userId, profile.copy(quota = quota))
      }
      if (userToUpdate.isEmpty) throw CosmosProfileException.unknownUser(id)
    }
  }

  override def getUserGroup(id: ProfileId)(implicit c: Conn): Group =
    users.collectFirst {
      case (_, profile) if profile.id == id => profile.group
    }.getOrElse(NoGroup)

  override def setUserGroup(id: ProfileId, maybeGroup: Option[String])(implicit c: Conn) {
    users.synchronized {
      val newGroup = maybeGroup match {
        case Some(name) => getGroups.find(_.name == name).getOrElse(NoGroup)
        case None => NoGroup
      }
      val userToUpdate = users.find(_._2.id == id)
      userToUpdate.foreach {
        case (userId, profile) =>
          val oldGroup = profile.group
          users = users.updated(userId, profile.copy(group = newGroup))
          groupsWithUsers.synchronized {
            groupsWithUsers = groupsWithUsers.updated(newGroup, groupsWithUsers(newGroup) + userId)
            groupsWithUsers = groupsWithUsers.updated(oldGroup, groupsWithUsers(oldGroup) - userId)
          }
      }
      if (userToUpdate.isEmpty) throw CosmosProfileException.unknownUser(id)
    }
  }

  override def enableUserCapability(id: ProfileId, capability: Capability)(implicit c: Conn) {
    updateProfile(id) { profile =>
      profile.copy(capabilities = profile.capabilities + capability)
    }
  }

  override def disableUserCapability(id: ProfileId, capability: Capability)(implicit c: Conn) {
    updateProfile(id) { profile =>
      profile.copy(capabilities = profile.capabilities - capability)
    }
  }

  override def getUserCapabilities(id: ProfileId)(implicit c: Conn): UserCapabilities = (for {
    profile <- users.values.find(_.id == id)
  } yield profile.capabilities).getOrElse(UntrustedUserCapabilities)

  override def handleExists(handle: String)(implicit c: Conn): Boolean =
    users.values.exists(_.handle == handle)

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
    groupsWithUsers(group).map(users).toSet

  override def lookupByHandle(handle: String)(implicit c: Conn): Option[CosmosProfile] =
    users.values.find(_.handle == handle)

  override def getGroups(implicit c: Conn): Set[Group] = groupsWithUsers.keys.toSet

  override def registerGroup(group: Group)(implicit c: Conn) {
    groupsWithUsers.synchronized { groupsWithUsers += (group -> Set()) }
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
}

object MockCosmosProfileDao {
  object DummyConnection
}
