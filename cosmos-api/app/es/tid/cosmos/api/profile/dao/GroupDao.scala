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

import es.tid.cosmos.api.quota.{GuaranteedGroup, LimitedQuota, Group}

trait GroupDao[Conn] {

  /** Find a guaranteed group by name.
    *
    * @param name group name
    * @param c    the connection to use
    * @return     a guaranteed group or none
    */
  def lookupByName(name: String)(implicit c: Conn): Option[GuaranteedGroup]

  /** Register a user group in the database.
    *
    * @param group the group to be persisted
    * @param c     the connection to use
    */
  def register(group: Group)(implicit c: Conn): Unit

  /** Delete a group by its name.
    *
    * @param name the group's name
    * @param c    the connection to use
    */
  def delete(name: String)(implicit c: Conn): Unit

  /** Get the user groups.
    *
    * @return the user groups including the NoGroup for profiles that do not belong to any group
    */
  def list()(implicit c: Conn): Set[Group]

  /** Set the minimum quota for the given group.
    *
    * @param name     the group's name
    * @param minQuota the minimum quota
    * @param c        the connection to use
    */
  def setQuota(name: String, minQuota: LimitedQuota)(implicit c: Conn): Unit
}
