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

import es.tid.cosmos.api.quota.{LimitedQuota, Group}

trait GroupDao[Conn] {

  /** Register a user group in the database.
    *
    * @param group the group to be persisted
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
