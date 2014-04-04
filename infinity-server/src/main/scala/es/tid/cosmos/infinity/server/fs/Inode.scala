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

sealed trait Inode {
  val id: String
  val permissions: FilePermissions
  val isDirectory: Boolean

  def canRead(user: UserProfile): Boolean = permissions.canRead(user)
  def canWrite(user: UserProfile): Boolean = permissions.canWrite(user)
  def canExec(user: UserProfile): Boolean = permissions.canExec(user)
}

sealed trait DirectoryInode extends Inode {

  override val isDirectory = true

  def newFile(name: String, permissions: FilePermissions = permissions) =
    FileInode(id = Inode.randomId(), parentId = id, name, permissions)

  def newDirectory(name: String, permissions: FilePermissions = permissions) =
    SubDirectoryInode(id = Inode.randomId(), parentId = id, name, permissions)
}

case class RootInode(permissions: FilePermissions) extends DirectoryInode {
  override val id: String = RootInode.Id
}
object RootInode {
  val Id = "0"
  val Name = "/"
}

sealed trait ChildInode extends Inode {
  val parentId: String
  val name: String
  require(id != parentId)
  require(!name.contains("/"))
}

case class FileInode(
    id: String, parentId: String, name: String, permissions: FilePermissions) extends ChildInode {
  override val isDirectory = false
}

case class SubDirectoryInode(
    id: String, parentId: String, name: String, permissions: FilePermissions)
  extends DirectoryInode with ChildInode

object Inode {
  def randomId() = UUID.randomUUID().toString.replace("-", "")
}
