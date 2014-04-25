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

package es.tid.comos.infinity.client

import java.io.{InputStreamReader, OutputStreamWriter}
import scala.concurrent.Future

import es.tid.cosmos.infinity.common.{SubPath, Path}
import es.tid.cosmos.infinity.common.messages.PathMetadata
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
      replication: Option[Int] = None,
      blockSize: Option[Int] = None): Future[Unit]

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
  def move(originPath: Path, targetPath: Path): Future[Unit]

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
  def changeOwner(path: Path, mask: PermissionsMask): Future[Unit]

  /** Delete a file or directory.
    *
    * @param path  Path to delete
    * @return      Success or failure as a future. Apart from the common exceptions, fail with
    *              NotFoundException when the path to delete doesn't exist
    */
  def delete(path: Path): Future[Unit]

  /** Retrieve file contents.
    *
    * @param path    Path of the file to read
    * @param offset  Optionally, where to start reading
    * @param length  Optionally, how much to retrieve
    * @return        A stream with the contents on success or fail with the common exceptions or
    *                NotFoundException when the path to read doesn't exist
    */
  def read(path: Path, offset: Option[Long], length: Option[Long]): Future[InputStreamReader]

  /** Append data to a file.
    *
    * @param path  Path of the file to append to
    * @return      A stream to write to, fail with the common exceptions or with NotFoundException
    *              when the path to write doesn't exist. Note that the returned stream can fail
    *              wrapping in IOException any common exception
    */
  def append(path: Path): Future[OutputStreamWriter]

  /** Overwrite a file.
    *
    * @param path  Path of the file to be overwritten
    * @return      A stream to write to, fail with the common exceptions or with NotFoundException
    *              when the path to write doesn't exist. Note that the returned stream can fail
    *              wrapping in IOException any common exception
    */
  def overwrite(path: Path): Future[OutputStreamWriter]
}
