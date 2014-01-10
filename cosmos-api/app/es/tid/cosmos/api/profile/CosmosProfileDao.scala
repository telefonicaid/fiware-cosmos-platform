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
import es.tid.cosmos.api.quota.{Group, LimitedQuota, Quota}
import es.tid.cosmos.servicemanager.clusters.ClusterId

/** This trait creates an abstraction on how to use the underlying data, such as SQL databases,
  * in-memory lists, mocks, etc.
  */
trait CosmosProfileDao {

  /** Specifies the type of connection to use to the data. */
  type Conn

  def withConnection[A](block: Conn => A): A
  def withTransaction[A](block: Conn => A): A

  /** Registers a new user.
    *
    * The user group, quota, and capabilities are set to default values.
    *
    * @param userId  The id specified by the user.
    * @param reg     The registration options.
    * @param state   The state of the user.
    * @param c       The connection to use.
    * @return        A newly created Cosmos profile.
    */
  def registerUser(userId: UserId, reg: Registration, state: UserState)
                  (implicit c: Conn): CosmosProfile

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
    * @throws CosmosProfileException  When no user has such id
    */
  def setMachineQuota(id: ProfileId, quota: Quota)(implicit c: Conn): Unit

  /** Enable some capability to the given user.
    *
    * If the capability was already enabled, nothing is done.
    *
    * @param id           The unique Cosmos ID of the given user.
    * @param capability   The capability to be enabled.
    * @throws CosmosProfileException  When no user has such id
    */
  def enableUserCapability(id: ProfileId, capability: Capability.Value)(implicit c: Conn): Unit

  /** Disable some capability to the given user.
    *
    * If the capability was already disabled, nothing is done.
    *
    * @param id           The unique Cosmos ID of the given user.
    * @param capability   The capability to be disabled.
    */
  def disableUserCapability(id: ProfileId, capability: Capability.Value)(implicit c: Conn): Unit

  /** Determines whether a handle is already taken.
    *
    * @param handle  Handle to check availability for.
    * @param c       The connection to use.
    * @return        Whether the handle is taken.
    */
  def handleExists(handle: String)(implicit c: Conn): Boolean

  /** Set the handle of a user. Fails if the new handle is in use.
    *
    * @param id      The id of the user
    * @param c       The connection to use
    * @param handle  The new handle
    * @throws CosmosProfileException
    *                When no user has such id or handle is already in use
    */
  def setHandle(id: ProfileId, handle: String)(implicit c: Conn): Unit

  /** Set the email of a user.
    *
    * @param id     The id of the user.
    * @param c      The connection to use.
    * @param email  The new email.
    * @throws CosmosProfileException  When no user has such id
    */
  def setEmail(id: ProfileId, email: String)(implicit c: Conn): Unit

  /** Set the state of a given user.
    *
    * @param id         Id of the user.
    * @param userState  The new state.
    * @param c          The connection to use.
    * @throws CosmosProfileException  When no user has such id
    */
  def setUserState(id: ProfileId, userState: UserState)(implicit c: Conn): Unit

  /** Obtains the profile for a given user.
    *
    * @param id  The id of who to retrieve the profile
    * @param c   The connection to use
    * @return    The profile of the given user
    */
  def lookupByProfileId(id: ProfileId)(implicit c: Conn): Option[CosmosProfile]

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

  /** Obtains the profiles that belong to a given group.
    *
    * @param group the group
    * @return      the profiles that belong to that group
    */
  def lookupByGroup(group: Group)(implicit c: Conn): Set[CosmosProfile]

  /** Obtains the profile of a given user.
    *
    * @param handle The handle
    * @param c      The connection to use
    * @return       The user with that handle or none
    */
  def lookupByHandle(handle: String)(implicit c: Conn): Option[CosmosProfile]

  /** Register a user group in the database.
    *
    * @param group the group to be persisted
    */
  def registerGroup(group: Group)(implicit c: Conn): Unit

  /** Delete a group by its name.
    *
    * @param name the group's name
    * @param c    the connection to use
    */
  def deleteGroup(name: String)(implicit c: Conn): Unit

  /** Get the user groups.
    *
    * @return the user groups including the NoGroup for profiles that do not belong to any group
    */
  def getGroups(implicit c: Conn): Set[Group]

  /** Set the group the user belongs to.
    *
    * @param id        the profile id of the user
    * @param groupName the optional group name. If `None` then the user will not belong to any group
    *                  indicated by setting it to [[es.tid.cosmos.api.profile.NoGroup]]
    * @param c         the connection to use
    */
  def setGroup(id: ProfileId, groupName: Option[String])(implicit c: Conn): Unit

  /** Set the minimum quota for the given group.
    *
    * @param name     the group's name
    * @param minQuota the minimum quota
    * @param c        the connection to use
    */
  def setGroupQuota(name: String, minQuota: LimitedQuota)(implicit c: Conn): Unit

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

  /** Determines the owner of a cluster if any.
    *
    * @param clusterId  Id of the cluster whose ownership is looked for
    * @param c          The connection to use
    * @return           The owner profile id or none
    */
  def ownerOf(clusterId: ClusterId)(implicit c: Conn): Option[ProfileId]

  /** Retrieves the set of cluster ids for a given user.
    *
    * @param id  The unique Cosmos ID of the given user.
    * @param c   The connection to use.
    * @return    The set of assigned clusters for a given user.
    * @throws CosmosProfileException  When no user has such id
    */
  def clustersOf(id: ProfileId)(implicit c: Conn): Seq[ClusterAssignment]

  /** Replace existing user public keys by the passed ones.
    *
    * @param id          The id of the user.
    * @param publicKeys  New public keys.
    * @throws CosmosProfileException  When no user has such id
    */
  def setPublicKeys(id: ProfileId, publicKeys: Seq[NamedKey])(implicit c: Conn): Unit
}
