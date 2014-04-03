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

import java.util.UUID

import es.tid.cosmos.infinity.server.authentication.UserProfile
import es.tid.cosmos.infinity.server.authorization.FilePermissions
import es.tid.cosmos.infinity.server.authorization.UnixFilePermissions._

/** Represents files and directories */
case class Inode(
    id: String,
    name: String,
    isDirectory: Boolean,
    permissions: FilePermissions,
    parentId: String) {

  val isRoot: Boolean = id == Inode.RootId

  def canRead(user: UserProfile): Boolean = permissions.canRead(user)
  def canWrite(user: UserProfile): Boolean = permissions.canWrite(user)
  def canExec(user: UserProfile): Boolean = permissions.canExec(user)

  def newChild(name: String, isDirectory: Boolean, user: UserProfile,
             permissions: FilePermissions = permissions) = {
    if (!this.isDirectory) {
      throw new UnsupportedOperationException(
        s"cannot create new child inode from non-directory $this")
    }
    new Inode(Inode.randomId(), name, isDirectory, permissions, id)
  }

  override def toString = s"Inode[$id, $name, $isDirectory, $permissions, $parentId]"
}

object Inode {
  private def randomId() = UUID.randomUUID().toString.replace("-", "")

  val RootId = "0"
  val RootName = "/"
}
