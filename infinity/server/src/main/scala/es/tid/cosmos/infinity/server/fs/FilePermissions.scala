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

package es.tid.cosmos.infinity.server.fs

import es.tid.cosmos.infinity.common.UserProfile
import es.tid.cosmos.infinity.common.permissions.{PermissionsMask, PermissionClass}

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
