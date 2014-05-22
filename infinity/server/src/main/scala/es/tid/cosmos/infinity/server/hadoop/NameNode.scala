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

package es.tid.cosmos.infinity.server.hadoop

import es.tid.cosmos.infinity.common.fs.{Path, PathMetadata}
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

/** An abstraction for an HDFS name node service. */
trait NameNode extends UserPrivileges {

  /** Retrieve the file or directory metadata corresponding to the given path.
    *
    * @param path The path whose metadata is to be obtained.
    * @return a FileMetadata if path contains a file, or DirectoryMetadata if it contains a directory.
    */
  @throws[NameNodeException.IOError]
  @throws[NameNodeException.Unauthorized]
  @throws[NameNodeException.NoSuchPath]
  def pathMetadata(path: Path): PathMetadata

  /** Create a new file on the given path.
    *
    * @param path The path where the new file is to be created
    * @param owner The owner of the new file
    * @param group The group of the new file
    * @param permissions The permissions for the new file
    * @param replication The replication factor for the new file
    * @param blockSize The block size for the new file
    */
  @throws[NameNodeException.IOError]
  @throws[NameNodeException.PathAlreadyExists]
  @throws[NameNodeException.ParentNotDirectory]
  @throws[NameNodeException.Unauthorized]
  @throws[NameNodeException.NoSuchPath]
  def createFile(
    path: Path,
    owner: String,
    group: String,
    permissions: PermissionsMask,
    replication: Option[Short],
    blockSize: Option[Long]): Unit

  /** Create a new directory on the given path.
    *
    * @param path The path where the new directory is to be created
    * @param owner The owner of the new directory
    * @param group The group of the new directory
    * @param permissions The permissions for the new directory
    * @throws NameNodeException.NoSuchPath if some of the parents in the path doesn't exist
    * @throws NameNodeException.PathAlreadyExists if there is already a file or directory on
    *         the given path
    * @throws NameNodeException.ParentNotDirectory if the parent path is not a directory
    * @throws NameNodeException.Unauthorized if the current user is not authorized to do the
    *         action
    * @throws NameNodeException.IOError if there is an unexpected IO error
    */
  def createDirectory(
     path: Path,
     owner: String,
     group: String,
     permissions: PermissionsMask): Unit

  /** Delete the file or directory on the given path.
    *
    * @param path The path of the file or directory to be removed.
    * @param recursive Indicates whether contents must be removed recursively in case of directory
    *                  (ignored for files).
    * @throws NameNodeException.NoSuchPath if there is no file or directory on the given path
    * @throws NameNodeException.Unauthorized if the current user is not authorized to do the
    *         action
    * @throws NameNodeException.IOError if there is an unexpected IO error
    */
  def deletePath(path: Path, recursive: Boolean): Unit

  /** Move the file or directory located in the given path to a new one.
    *
    * @param from The path of the file or directory to be moved.
    * @param to The path of the new location of the file or directory.
    * @throws NameNodeException.NoSuchPath if there is no file or directory on the path
    *         indicated by `from` argument
    * @throws NameNodeException.PathAlreadyExists if there is already a file or directory
    *         in the path indicated by `to` argument
    * @throws NameNodeException.Unauthorized if the current user is not authorized to do the
    *         action
    * @throws NameNodeException.IOError if there is an unexpected IO error
    */
  def movePath(from: Path, to: Path): Unit

  /** Set the owner of a given file or directory.
    *
    * @param path The path to the file or directory whose owner is to be set
    * @param newOwner The new owner of the file or directory
    * @throws NameNodeException.NoSuchPath if there is no file or directory on the given path
    * @throws NameNodeException.Unauthorized if the current user is not authorized to do the
    *         action
    * @throws NameNodeException.IOError if there is an unexpected IO error
    */
  def setOwner(path: Path, newOwner: String): Unit

  /** Set the group of a given file or directory.
    *
    * @param path The path to the file or directory whose group is to be set
    * @param newGroup The new group of the file or directory
    * @throws NameNodeException.NoSuchPath if there is no file or directory on the given path
    * @throws NameNodeException.Unauthorized if the current user is not authorized to do the
    *         action
    * @throws NameNodeException.IOError if there is an unexpected IO error
    */
  def setGroup(path: Path, newGroup: String): Unit

  /** Set the permissions for a given file or directory.
    *
    * @param path The path to the file or directory whose group is to be set
    * @param permissions The new permissions of the file or directory
    * @throws NameNodeException.NoSuchPath if there is no file or directory on the given path
    * @throws NameNodeException.Unauthorized if the current user is not authorized to do the
    *           action
    * @throws NameNodeException.IOError if there is an unexpected IO error
    */
  def setPermissions(path: Path, permissions: PermissionsMask): Unit
}
