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

import es.tid.cosmos.api.authorization.ApiCredentials
import es.tid.cosmos.api.controllers.pages.{NamedKey, CosmosProfile, Registration}

trait MockCosmosProfileDaoComponent extends CosmosProfileDaoComponent {
  def cosmosProfileDao: CosmosProfileDao = new MockCosmosProfileDao
}

/**
 * Mock to be used in tests for handling the profile DAO. Not thread-safe.
 */
class MockCosmosProfileDao extends CosmosProfileDao {

  object DummyConnection
  type Conn = DummyConnection.type

  private var users = Map[UserId, CosmosProfile]()
  private var clusters = List[ClusterAssignation]()

  def withConnection[A](block: (Conn) => A): A = block(DummyConnection)
  def withTransaction[A](block: (Conn) => A): A = block(DummyConnection)

  override def registerUserInDatabase(userId: UserId, reg: Registration)(implicit c: Conn): Long = {
    val credentials = ApiCredentials.random()
    val cosmosId: Long = users.size
    users = users.updated(userId, CosmosProfile(
      id = cosmosId,
      handle = reg.handle,
      quota = UnlimitedQuota,
      apiCredentials = credentials,
      keys = List(NamedKey("default", reg.publicKey))
    ))
    cosmosId
  }

  override def getCosmosId(userId: UserId)(implicit c: Conn): Option[Long] =
    users.get(userId).map(_.id)

  override def getMachineQuota(cosmosId: Long)(implicit c: Conn): Quota =
    users.collectFirst {
      case (_, CosmosProfile(`cosmosId`, _, quota, _, _)) => quota
    }.getOrElse(EmptyQuota)

  override def setMachineQuota(cosmosId: Long, quota: Quota)(implicit c: Conn): Boolean = synchronized {
    users.collectFirst {
      case (userId, profile @ CosmosProfile(`cosmosId`, _, _, _, _)) => {
        users = users.updated(userId, profile.copy(quota = quota))
        true
    }}.getOrElse(false)
  }

  override def handleExists(handle: String)(implicit c: Conn): Boolean =
    users.values.exists(_.handle == handle)

  override def lookupByUserId(userId: UserId)(implicit c: Conn): Option[CosmosProfile] =
    users.get(userId)

  override def lookupByApiCredentials(creds: ApiCredentials)(implicit c: Conn): Option[CosmosProfile] =
    users.collectFirst {
      case (_, profile@CosmosProfile(_, _, _, `creds`, _)) => profile
    }

  override def assignCluster(assignment: ClusterAssignation)(implicit c: Conn) {
    synchronized {
      require(!clusters.exists(_.clusterId == assignment.clusterId), "Cluster already assigned")
      clusters = clusters :+ assignment
    }
  }

  override def clustersOf(cosmosId: Long)(implicit c: Conn): Seq[ClusterAssignation] =
    clusters.filter(_.ownerId == cosmosId)
}
