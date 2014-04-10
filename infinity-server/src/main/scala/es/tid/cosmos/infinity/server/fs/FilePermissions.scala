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

package es.tid.cosmos.infinity.server.fs

import es.tid.cosmos.infinity.server.authentication.UserProfile
import es.tid.cosmos.infinity.server.permissions.{PermissionsMask, PermissionClass}

/** The permissions of an Inode in Infinity. */
case class FilePermissions(
  owner: String,
  group: String,
  unix: PermissionsMask) {

  def canRead(user: UserProfile): Boolean = can(user, p => p.read)

  def canWrite(user: UserProfile): Boolean = can(user, p => p.write)

  def canExec(user: UserProfile): Boolean = can(user, p => p.execute)

  private def can(user: UserProfile, op: PermissionClass => Boolean) =
    user.superuser ||
      (user.username == owner && op(unix.owner) && op(user.mask.owner)) ||
      (user.group == group && op(unix.group) && op(user.mask.group)) ||
      (op(unix.others) && op(user.mask.others))
}
