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

import es.tid.cosmos.infinity.common.Path

sealed abstract class AccessException(message: String) extends Exception(message)

case class PermissionDenied(path: Path) extends AccessException(s"Permission denied on $path")
case class NoSuchInode(id: String) extends AccessException(s"No inode with id='$id'")
case class NoSuchFileOrDirectory(path: Path)
  extends AccessException(s"No such file or directory: $path")
case class DirectoryNonEmpty(path: Path) extends AccessException(s"Directory $path is not empty")
case class InvalidOperation(path: Path) extends AccessException(s"Invalid operation on $path")
case class PathAlreadyExists(path: Path) extends AccessException(s"$path already exists")
