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

package es.tid.cosmos.api.profile.dao

import es.tid.cosmos.api.quota.{GuaranteedGroup, NoGroup, Quota, Group}
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.UserState.UserState
import es.tid.cosmos.api.profile.Registration

trait ProfileDao[Conn] {

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
  def register(userId: UserId, reg: Registration, state: UserState)(implicit c: Conn): CosmosProfile

  /** Retrieves all the Cosmos profiles.
    *
    * @param c  The connection to use.
    * @return   All the profiles.
    */
  def list()(implicit c: Conn): Seq[CosmosProfile]

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
    * @throws CosmosDaoException
    *                When no user has such id or handle is already in use
    */
  def setHandle(id: ProfileId, handle: String)(implicit c: Conn): Unit

  /** Set the email of a user.
    *
    * @param id     The id of the user.
    * @param c      The connection to use.
    * @param email  The new email.
    * @throws CosmosDaoException  When no user has such id
    */
  def setEmail(id: ProfileId, email: String)(implicit c: Conn): Unit

  /** Replace existing user public keys by the passed ones.
    *
    * @param id          The id of the user.
    * @param publicKeys  New public keys.
    * @throws CosmosDaoException  When no user has such id
    */
  def setPublicKeys(id: ProfileId, publicKeys: Seq[NamedKey])(implicit c: Conn): Unit

  /** Set the state of a given user.
    *
    * @param id         Id of the user.
    * @param userState  The new state.
    * @param c          The connection to use.
    * @throws CosmosDaoException  When no user has such id
    */
  def setUserState(id: ProfileId, userState: UserState)(implicit c: Conn): Unit

  /** Set the group the user belongs to.
    *
    * @param id        the profile id of the user
    * @param groupName the optional group name. If `None` then the user will not belong to any group
    *                  indicated by setting it to [[es.tid.cosmos.api.quota.NoGroup]]
    * @param c         the connection to use
    */
  def setGroup(id: ProfileId, groupName: Option[String])(implicit c: Conn): Unit

  /** Set the group the user belongs to.
    *
    * @param id      the profile id of the user
    * @param group   the new user group
    * @param c       the connection to use
    */
  def setGroup(id: ProfileId, group: Group)(implicit c: Conn): Unit = setGroup(id, group match {
    case NoGroup => None
    case GuaranteedGroup(name, _) => Some(name)
  })

  /** Sets the machine quota for a given user.
    *
    * @param id     The unique Cosmos ID of the given user.
    * @param quota  The quota to set.
    * @param c      The connection to use.
    * @throws CosmosDaoException  When no user has such id
    */
  def setMachineQuota(id: ProfileId, quota: Quota)(implicit c: Conn): Unit
}
