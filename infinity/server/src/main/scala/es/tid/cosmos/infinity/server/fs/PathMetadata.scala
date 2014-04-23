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

import java.net.URL

import es.tid.cosmos.infinity.common.Path
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

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
