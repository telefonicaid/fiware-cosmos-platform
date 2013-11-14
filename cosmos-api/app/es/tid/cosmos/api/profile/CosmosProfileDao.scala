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

import java.util.Date

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.profile.UserState.UserState
import es.tid.cosmos.servicemanager.clusters.ClusterId

/** This trait creates an abstraction on how to use the underlying data, such as SQL databases,
  * in-memory lists, mocks, etc.
  */
trait CosmosProfileDao {

  /** Specifies the type of connection to use to the data.
    */
  type Conn

  /** Identifier of the Cosmos profile.
    * 
    * Not to confuse with the user id of the authentication provider whose lifecycle is different
    * from the Cosmos profile.
    */
  type ProfileId = Long

  def withConnection[A](block: Conn => A): A
  def withTransaction[A](block: Conn => A): A

  /** Registers a new user in the database.
    *
    * @param userId  The id specified by the user.
    * @param reg     The registration options.
    * @param c       The connection to use.
    * @return        A newly created Cosmos profile.
    */
  def registerUser(userId: UserId, reg: Registration)(implicit c: Conn): CosmosProfile

  /** Retrieves all the Cosmos profiles.
    *
    * @param c  The connection to use.
    * @return   All the profiles.
    */
  def getAllUsers()(implicit c: Conn): Seq[CosmosProfile]

  /** Retrieves the unique Cosmos ID for a given user.
    *
    * @param userId  The id specified by the user.
    * @param c       The connection to use.
    * @return        The unique Cosmos ID for a given user.
    */
  def getProfileId(userId: UserId)(implicit c: Conn): Option[ProfileId]

  /** Retrieves the machine quota for a given user.
    *
    * @param id  The unique Cosmos ID of the given user.
    * @param c   The connection to use.
    * @return    The quota that the user has.
    */
  def getMachineQuota(id: ProfileId)(implicit c: Conn): Quota

  /** Sets the machine quota for a given user.
    *
    * @param id     The unique Cosmos ID of the given user.
    * @param quota  The quota to set.
    * @param c      The connection to use.
    * @return       Whether the operation succeeded or not.
    */
  def setMachineQuota(id: ProfileId, quota: Quota)(implicit c: Conn): Boolean

  /** Determines whether a handle is already taken.
    *
    * @param handle  Handle to check availability for.
    * @param c       The connection to use.
    * @return        Whether the handle is taken.
    */
  def handleExists(handle: String)(implicit c: Conn): Boolean

  /** Set the handle of a user. Fails if the new handle is in use.
    *
    * @param id      The id of the user.
    * @param c       The connection to use.
    * @param handle  The new handle.
    */
  def setHandle(id: ProfileId, handle: String)(implicit c: Conn): Unit

  /** Set the email of a user.
    *
    * @param id     The id of the user.
    * @param c      The connection to use.
    * @param email  The new email.
    */
  def setEmail(id: ProfileId, email: String)(implicit c: Conn): Unit
  
  /** Set the state of a given user.
    *
    * @param id         Id of the user.
    * @param userState  The new state.
    * @param c          The connection to use.
    */
  def setUserState(id: ProfileId, userState: UserState)(implicit c: Conn): Unit

  /** Obtains the profile for a given user.
    *
    * @param userId  The user id of who to retrieve the profile.
    * @param c       The connection to use.
    * @return        The profile of the given user.
    */
  def lookupByUserId(userId: UserId)(implicit c: Conn): Option[CosmosProfile]

  /** Obtains the profile for a given set of credentials.
    *
    * @param creds  The credentials used to retrieve the profile.
    * @param c      The connection to use.
    * @return       The profile of the given user.
    */
  def lookupByApiCredentials(creds: ApiCredentials)(implicit c: Conn): Option[CosmosProfile]

  /** Assigns a cluster to a given user at the present moment.
    * @param clusterId  The cluster ID to assign.
    * @param ownerId    The unique Cosmos ID of the new owner.
    * @param c          The connection to use.
    */
  def assignCluster(clusterId: ClusterId, ownerId: Long)(implicit c: Conn) {
    assignCluster(ClusterAssignment(clusterId, ownerId, new Date()))
  }

  /** Assigns a cluster to a given user.
    *
    * @param clusterId     The cluster ID to assign.
    * @param ownerId       The unique Cosmos ID of the new owner.
    * @param creationDate  The instant the cluster creation started.
    * @param c             The connection to use.
    */
  def assignCluster(clusterId: ClusterId, ownerId: Long, creationDate: Date)(implicit c: Conn) {
    assignCluster(ClusterAssignment(clusterId, ownerId, creationDate))
  }

  /** Assigns a cluster to a given user.
    *
    * @param assignment  Assignment to make persistent.
    * @param c           The connection to use.
    */
  def assignCluster(assignment: ClusterAssignment)(implicit c: Conn): Unit

  /** Retrieves the set of cluster ids for a given user.
    *
    * @param id  The unique Cosmos ID of the given user.
    * @param c   The connection to use.
    * @return    The set of assigned clusters for a given user.
    */
  def clustersOf(id: ProfileId)(implicit c: Conn): Seq[ClusterAssignment]

  /**
   * Replace existing user public keys by the passed ones.
   *
   * @param id          The id of the user.
   * @param publicKeys  New public keys.
   */
  def setPublicKeys(id: ProfileId, publicKeys: Seq[NamedKey])(implicit c: Conn): Unit
}
