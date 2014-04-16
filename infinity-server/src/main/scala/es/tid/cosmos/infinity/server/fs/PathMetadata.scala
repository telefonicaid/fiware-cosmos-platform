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

import java.net.URL

import es.tid.cosmos.infinity.server.permissions.PermissionsMask
import es.tid.cosmos.infinity.server.util.Path

trait PathMetadata

/** The metadata of a Infinity file.
  *
  * @constructor
  * @param path The path where the file is located
  * @param metadata The URL to the metadata resource
  * @param content The URL to the most appropriate content server where data is available
  * @param owner The username of the file's owner
  * @param group The group this file is assigned to
  * @param modificationTime The seconds since the epoch when the file was lastly modified
  * @param accessTime The seconds from the epoch when the file was lastly accessed
  * @param replication The replication factor
  * @param blocksize The block size
  * @param permissions The permissions of the file
  * @param size The size of the file
  */
case class FileMetadata(
  path: Path,
  metadata: URL,
  content: URL,
  owner: String,
  group: String,
  modificationTime: Long,
  accessTime: Long,
  replication: Int,
  blocksize: Long,
  permissions: PermissionsMask,
  size: Long) extends PathMetadata {
  require(replication > 0, s"replication factor must be > 0, but $replication given")
  require(blocksize> 0, s"block size must be > 0, but $blocksize given")
}
