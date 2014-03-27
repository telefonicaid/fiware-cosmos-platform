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
    directory: Boolean,
    permissions: FilePermissions,
    parentId: String) {

  def canRead(user: UserProfile): Boolean = permissions.canRead(user)
  def canWrite(user: UserProfile): Boolean = permissions.canWrite(user)
  def canExec(user: UserProfile): Boolean = permissions.canExec(user)

  def update(name: Option[String] = None, permissions: Option[FilePermissions] = None,
            parent: Option[String] = None, user: UserProfile): Inode = {
    require(canWrite(user), "Permission denied")
    new Inode(id, name.getOrElse(this.name), directory,
      permissions.getOrElse(this.permissions), parent.getOrElse(this.parentId))
  }

  def create(name: String, directory: Boolean, user: UserProfile,
             permissions: FilePermissions = permissions) = {
    require(this.directory, "No such file or directory")
    require(canWrite(user), "Permission denied")
    new Inode(Inode.randomId(), name, directory, permissions, id)
  }

  override def toString = s"Inode[$id, $name, $directory, $permissions, $parentId]"
}

object Inode {
  private def randomId() = UUID.randomUUID().toString.replace("-", "")
}

object RootInode extends Inode (
  id = "0",
  name = "/",
  directory = true,
  permissions = FilePermissions("root", "root", fromOctal("777")),
  parentId = "0"
)
