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

trait MockCosmosProfileDaoComponent extends CosmosProfileDaoComponent {
  def cosmosProfileDao: CosmosProfileDao = new MockCosmosProfileDao
}

/**
 * Mock to be used in tests for handling the profile DAO. Not thread-safe.
 */
class MockCosmosProfileDao extends CosmosProfileDao {

  object DummyConnection
  type Conn = DummyConnection.type

  @volatile private var users = Map[UserId, CosmosProfile]()
  @volatile private var clusters = List[ClusterAssignment]()
  @volatile private var groupsWithUsers = Map[Group, Set[UserId]](NoGroup -> Set())

  def withConnection[A](block: (Conn) => A): A = block(DummyConnection)
  def withTransaction[A](block: (Conn) => A): A = block(DummyConnection)

  override def registerUserInDatabase(userId: UserId, reg: Registration, group: Group, quota: Quota)
                                     (implicit c: Conn): CosmosProfile = { //FIXME
    val credentials = ApiCredentials.random()
    require(!users.values.exists(_.handle == reg.handle), s"Duplicated handle: ${reg.handle}")
    require(groupsWithUsers.contains(group), s"Group not registered: $group")
    val cosmosProfile = CosmosProfile(
      id = users.size,
      state = Enabled,
      handle = reg.handle,
      email = reg.email,
      group,
      quota,
      apiCredentials = credentials,
      keys = List(NamedKey("default", reg.publicKey))
    )
    users.synchronized{ users = users.updated(userId, cosmosProfile) }
    groupsWithUsers.synchronized{
      groupsWithUsers = groupsWithUsers.updated(group, groupsWithUsers(group) + userId)
    }
    cosmosProfile
  }

  override def getCosmosId(userId: UserId)(implicit c: Conn): Option[Long] =
    users.get(userId).map(_.id)

  override def getMachineQuota(cosmosId: Long)(implicit c: Conn): Quota =
    users.collectFirst {
      case (_, profile) if profile.id == cosmosId => profile.quota
    }.getOrElse(EmptyQuota)

  override def setMachineQuota(cosmosId: Long, quota: Quota)
                              (implicit c: Conn): Boolean = synchronized {
    users.collectFirst {
      case (userId, profile) if profile.id == cosmosId => {
        users = users.updated(userId, profile.copy(quota = quota))
        true
    }}.getOrElse(false)
  }

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
    synchronized {
      require(!clusters.exists(_.clusterId == assignment.clusterId), "Cluster already assigned")
      clusters.synchronized{ clusters = clusters :+ assignment }
    }
  }

  override def clustersOf(cosmosId: Long)(implicit c: Conn): Seq[ClusterAssignment] =
    clusters.filter(_.ownerId == cosmosId)

  override def setHandle(id: Long, handle: String)(implicit c: Conn) {
    updateProfile(id) { profile =>
      require(
        profile.handle == handle || users.values.find(_.handle == handle).isEmpty,
        "duplicated handle"
      )
      profile.copy(handle = handle)
    }
  }

  override def setUserState(id: Long, userState: UserState)(implicit c: Conn) {
    updateProfile(id) { profile =>
      profile.copy(state = userState)
    }
  }

  override def setPublicKeys(id: Long, publicKeys: Seq[NamedKey])(implicit c: Conn) {
    updateProfile(id) { profile =>
      profile.copy(keys = publicKeys)
    }
  }

  override def lookupByGroup(group: Group)(implicit c: Conn): Set[CosmosProfile] =
    groupsWithUsers(group).map(users).toSet

  override def getGroups(implicit c: Conn): Set[Group] = groupsWithUsers.keys.toSet

  override def registerGroup(group: Group)(implicit c: Conn) {
    groupsWithUsers.synchronized { groupsWithUsers += (group -> Set()) }
  }

  private def updateProfile(id: Long)(f: CosmosProfile => CosmosProfile) {
    val maybeId = users.collectFirst {
      case (userId, profile) if profile.id == id => userId
    }
    maybeId.map(userId =>
      users.synchronized{ users = users.updated(userId, f(users(userId))) }
    ).getOrElse(throw new IllegalArgumentException(s"No user with id=$id"))
  }
}
