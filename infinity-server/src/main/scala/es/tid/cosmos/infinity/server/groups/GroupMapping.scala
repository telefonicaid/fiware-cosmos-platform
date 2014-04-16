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

package es.tid.cosmos.infinity.server.groups

private[groups] trait GroupMapping {

  /** Groups of an user.
    *
    * The list must have at least one group for existing users and be empty for unknown ones.
    */
  type GroupList = Seq[String]

  /** Retrieve groups for a given user.
    *
    * @return None if the user doesn't exist or the list of groups otherwise
    */
  def groupsFor(handle: String): GroupList
}
