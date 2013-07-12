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
import es.tid.cosmos.servicemanager.ClusterId

trait MockCosmosProfileDaoComponent extends CosmosProfileDaoComponent {
  def cosmosProfileDao: CosmosProfileDao = new MockCosmosProfileDao
}

/**
 * Mock to be used in tests for handling the profile DAO. Not thread-safe.
 */
class MockCosmosProfileDao extends CosmosProfileDao {

  object DummyConnection
  type Conn = DummyConnection.type

  private var users = Map[String, CosmosProfile]()
  private var clusters = Map[ClusterId, Long]()

  def withConnection[A](block: (Conn) => A): A = block(DummyConnection)
  def withTransaction[A](block: (Conn) => A): A = block(DummyConnection)

  override def registerUserInDatabase(userId: String, reg: Registration)(implicit c: Conn): Long = {
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

  override def getCosmosId(userId: String)(implicit c: Conn): Option[Long] =
    users.get(userId).map(_.id)

  override def getMachineQuota(cosmosId: Long)(implicit c: Conn): Quota =
    users.collectFirst {
      case (_, CosmosProfile(`cosmosId`, _, quota, _, _)) => quota
    }.getOrElse(EmptyQuota)

  override def setMachineQuota(cosmosId: Long, quota: Quota)(implicit c: Conn): Boolean =
    users.collectFirst {
      case (userId, profile @ CosmosProfile(`cosmosId`, _, _, _, _)) => {
        users = users.updated(userId, profile.copy(quota = quota))
        true
    }}.getOrElse(false)

  override def lookupByUserId(userId: String)(implicit c: Conn): Option[CosmosProfile] =
    users.get(userId)

  override def lookupByApiCredentials(creds: ApiCredentials)(implicit c: Conn): Option[CosmosProfile] =
    users.collectFirst {
      case (_, profile@CosmosProfile(_, _, _, `creds`, _)) => profile
    }

  override def assignCluster(clusterId: ClusterId, ownerId: Long)(implicit c: Conn) {
    clusters = clusters.updated(clusterId, ownerId)
  }

  override def clustersOf(cosmosId: Long)(implicit c: Conn): Seq[ClusterId] =
    clusters.collect {
      case (clusterId, `cosmosId`) => clusterId
    }.toSeq
}
