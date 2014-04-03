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

import es.tid.cosmos.infinity.server.util.Path

sealed abstract class AccessException(message: String) extends Exception(message)

case class PermissionDenied(path: Path) extends AccessException(s"Permission denied on $path")
case class NoSuchInode(id: String) extends AccessException(s"No inode with id='$id'")
case class NoSuchFileOrDirectory(path: Path)
  extends AccessException(s"No such file or directory: $path")
case class DirectoryNonEmpty(path: Path) extends AccessException(s"Directory $path is not empty")
case class InvalidOperation(path: Path) extends AccessException(s"Invalid operation on $path")
case class PathAlreadyExists(path: Path) extends AccessException(s"$path already exists")
