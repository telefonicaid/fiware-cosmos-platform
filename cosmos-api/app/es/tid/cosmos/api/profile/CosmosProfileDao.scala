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
import es.tid.cosmos.api.controllers.pages._
import es.tid.cosmos.servicemanager.ClusterId

/**
 * This trait creates an abstraction on how to use the underlying data, such as SQL databases,
 * in-memory lists, mocks, etc.
 */
trait CosmosProfileDao {

  /**
   * Specifies the type of connection to use to the data.
   */
  type Conn

  def withConnection[A](block: Conn => A): A
  def withTransaction[A](block: Conn => A): A

  /**
   * Registers a new user in the database.
   *
   * @param userId  The id specified by the user.
   * @param reg     The registration options.
   * @param c       The connection to use.
   * @return        The unique Cosmos ID generated for the new user.
   */
  def registerUserInDatabase(userId: UserId, reg: Registration)(implicit c: Conn): Long

  /**
   * Retrieves the unique Cosmos ID for a given user.
   *
   * @param userId  The id specified by the user.
   * @param c       The connection to use.
   * @return        The unique Cosmos ID for a given user.
   */
  def getCosmosId(userId: UserId)(implicit c: Conn): Option[Long]

  /**
   * Retrieves the machine quota for a given user.
   *
   * @param cosmosId  The unique Cosmos ID of the given user.
   * @param c         The connection to use.
   * @return          The quota that the user has.
   */
  def getMachineQuota(cosmosId: Long)(implicit c: Conn): Quota

  /**
   * Sets the machine quota for a given user.
   *
   * @param cosmosId  The unique Cosmos ID of the given user.
   * @param quota     The quota to set.
   * @param c         The connection to use.
   * @return          Whether the operation succeeded or not.
   */
  def setMachineQuota(cosmosId: Long, quota: Quota)(implicit c: Conn): Boolean

  /**
   * Obtains the profile for a given user.
   *
   * @param userId  The user id of who to retrieve the profile.
   * @param c       The connection to use.
   * @return        The profile of the given user.
   */
  def lookupByUserId(userId: UserId)(implicit c: Conn): Option[CosmosProfile]

  /**
   * Obtains the profile for a given set of credentials.
   *
   * @param creds  The credentials used to retrieve the profile.
   * @param c      The connection to use.
   * @return       The profile of the given user.
   */
  def lookupByApiCredentials(creds: ApiCredentials)(implicit c: Conn): Option[CosmosProfile]

  /**
   * Assigns a cluster to a given user.
   * @param clusterId  The cluster ID to assign.
   * @param ownerId    The unique Cosmos ID of the new owner.
   * @param c          The connection to use.
   */
  def assignCluster(clusterId: ClusterId, ownerId: Long)(implicit c: Conn): Unit

  /**
   * Retrieves the set of cluster ids for a given user.
   * @param cosmosId  The unique Cosmos ID of the given user.
   * @param c         The connection to use.
   * @return          The set of cluster ids for a given user.
   */
  def clustersOf(cosmosId: Long)(implicit c: Conn): Seq[ClusterId]
}
