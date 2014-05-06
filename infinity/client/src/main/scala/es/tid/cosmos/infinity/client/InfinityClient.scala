/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.client

import java.io.{InputStreamReader, OutputStreamWriter}
import scala.concurrent.Future

import es.tid.cosmos.infinity.common.fs.{Path, PathMetadata, SubPath}
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

/** Infinity client API.
  *
  * API methods return Scala futures to provide asynchronous access to the Infinity server side.
  * When operations fail, they include an exception derived from InfinityException but you should
  * be prepared to catch exceptions produced by the ExecutionContext or by unexpected conditions.
  *
  * All API methods can fail with the following exceptions:
  *
  *  * ConnectionException        when it is not possible to contact the infinity server
  *  * UnauthorizedException      when credentials being used are rejected
  *  * ProtocolMismatchException  when client and server don't understand each other
  *  * ForbiddenException         when user access rights are not enough to perform the action
  *
  * Apart from those, every method can fail with specific exceptions.
  */
trait InfinityClient {

  /** Retrieves the metadata of a file or directory.
    *
    * @param path  Path to check
    * @return      Metadata of the path if it exists
    */
  def pathMetadata(path: Path): Future[Option[PathMetadata]]

  /** Create a new file.
    *
    * @param path         Path of the file to create. The parent must exist
    * @param permissions  Permissions mask for the new file
    * @param replication  Optionally, the replication factor
    * @param blockSize    Optionally, the block size of the file
    * @return             A successful future if file is created. Otherwise the common exceptions
    *                     and:
    *
    *                       * AlreadyExistsException  when the path already exists
    *                       * NotFoundException       when the parent path doesn't exist
    */
  def createFile(
      path: SubPath,
      permissions: PermissionsMask,
      replication: Option[Short] = None,
      blockSize: Option[Long] = None): Future[Unit]

  /** Create a new directory.
    *
    * @param path         Path of the file to create. The parent must exist
    * @param permissions  Permissions mask for the new file
    * @return             A successful future if it is created. Otherwise the common exceptions
    *                     and:
    *
    *                       * AlreadyExistsException  when the path already exists
    *                       * NotFoundException       when the parent path doesn't exist
    */
  def createDirectory(path: SubPath, permissions: PermissionsMask): Future[Unit]

  /** Move a file or directory.
    *
    * The movement should not create a loop (i.e. moving a directory to a subdirectory).
    *
    * @param originPath  Path to move
    * @param targetPath  Destination path that should exist as a directory
    * @return            A successful future if the path is moved. Otherwise the common exceptions
    *                    or AlreadyExistsException when the target path already exists
    */
  def move(originPath: SubPath, targetPath: Path): Future[Unit]

  /** Change path owner.
    *
    * @param path   Path to modify
    * @param owner  New owner
    * @return       Success or failure as a future
    */
  def changeOwner(path: Path, owner: String): Future[Unit]

  /** Change path group.
    *
    * @param path   Path to modify
    * @param group  New group
    * @return       Success or failure as a future
    */
  def changeGroup(path: Path, group: String): Future[Unit]

  /** Change path permissions mask.
    *
    * @param path  Path to modify
    * @param mask  New mask
    * @return      Success or failure as a future
    */
  def changePermissions(path: Path, mask: PermissionsMask): Future[Unit]

  /** Delete a file or directory.
    *
    * @param path        Path to delete
    * @param isRecursive perform a recursive deletion iff the path is a directory
    * @return            Success or failure as a future. Apart from the common exceptions, fail with
    *                    NotFoundException when the path to delete doesn't exist
    */
  def delete(path: SubPath, isRecursive: Boolean): Future[Unit]

  /** Retrieve file contents.
    *
    * @param path    Path of the file to read
    * @param offset  Optionally, where to start reading
    * @param length  Optionally, how much to retrieve
    * @return        A stream with the contents on success or fail with the common exceptions or
    *                NotFoundException when the path to read doesn't exist
    */
  def read(path: SubPath, offset: Option[Long], length: Option[Long]): Future[InputStreamReader]

  /** Append data to a file.
    *
    * @param path  Path of the file to append to
    * @return      A stream to write to, fail with the common exceptions or with NotFoundException
    *              when the path to write doesn't exist. Note that the returned stream can fail
    *              wrapping in IOException any common exception
    */
  def append(path: SubPath): Future[OutputStreamWriter]

  /** Overwrite a file.
    *
    * @param path  Path of the file to be overwritten
    * @return      A stream to write to, fail with the common exceptions or with NotFoundException
    *              when the path to write doesn't exist. Note that the returned stream can fail
    *              wrapping in IOException any common exception
    */
  def overwrite(path: SubPath): Future[OutputStreamWriter]
}
