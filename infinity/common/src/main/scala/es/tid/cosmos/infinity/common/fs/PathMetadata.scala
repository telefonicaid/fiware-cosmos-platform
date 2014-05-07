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

package es.tid.cosmos.infinity.common.fs

import java.net.URL
import java.util.Date

import es.tid.cosmos.infinity.common.permissions.PermissionsMask

/** Fields that any metadata object should have. */
sealed trait AbstractMetadata {
  val path: Path
  val `type`: PathType
  val metadata: URL
  val owner: String
  val group: String
  val modificationTime: Date
  val accessTime: Date
  val permissions: PermissionsMask
  val replication: Short
  val blockSize: Long
  val size: Long

  def isDirectory: Boolean = `type` == PathType.Directory
}

/** Metadata of either a file or a directory. */
sealed trait PathMetadata extends AbstractMetadata

case class FileMetadata(
    override val path: Path,
    override val metadata: URL,
    content: Option[URL],
    override val owner: String,
    override val group: String,
    override val modificationTime: Date,
    override val accessTime: Date,
    override val permissions: PermissionsMask,
    override val replication: Short,
    override val blockSize: Long,
    override val size: Long) extends PathMetadata {

  override val `type` = PathType.File

  def this(
      path: String, metadata: String, content: String, owner: String, group: String,
      modificationTime: Date, accessTime: Date, permissions: String, replication: Short,
      blockSize: Long, size: Long) =
    this(Path.absolute(path), new URL(metadata), Option(content).map(new URL(_)), owner, group,
      modificationTime, accessTime, PermissionsMask.fromOctal(permissions), replication, blockSize,
      size)

  require(replication >= 0, s"Replication must be non-negative but was $replication")
  require(blockSize > 0, s"Block size must be positive but was $blockSize")
  require(size >= 0, s"File size must be non-negative but was $size")
}

/** Metadata of a directory including an entry per child path. */
case class DirectoryMetadata(
    override val path: Path,
    override val metadata: URL,
    content: Seq[DirectoryEntry],
    override val owner: String,
    override val group: String,
    override val modificationTime: Date,
    override val accessTime: Date,
    override val permissions: PermissionsMask) extends PathMetadata {

  override val `type` = PathType.Directory
  override val replication = DirectoryMetadata.Replication
  override val blockSize = DirectoryMetadata.BlockSize
  override val size = DirectoryMetadata.Size

  def this(path: String, metadata: String, content: Seq[DirectoryEntry],
    owner: String, group: String, modificationTime: Date, accessTime: Date,
    permissions: String) = this(Path.absolute(path), new URL(metadata), content, owner, group,
    modificationTime, accessTime, PermissionsMask.fromOctal(permissions))
}

object DirectoryMetadata {
  val Replication: Short = 0
  val BlockSize: Long = 0L
  val Size: Long = 0L
}

/** Metadata for every directory entry, without the content field to avoid recursion. */
case class DirectoryEntry (
    override val path: Path,
    override val `type`: PathType,
    override val metadata: URL,
    override val owner: String,
    override val group: String,
    override val modificationTime: Date,
    override val accessTime: Date,
    override val permissions: PermissionsMask,
    override val replication: Short,
    override val blockSize: Long,
    override val size: Long) extends AbstractMetadata {

  require(size >= 0, s"File size must be non-negative but was $size")

  def this(path: String, `type`: String, metadata: String, owner: String, group: String,
           modificationTime: Date, accessTime: Date, permissions: String, replication: Short,
           blockSize: Long, size: Long) =
    this(Path.absolute(path), PathType.valueOf(`type`), new URL(metadata), owner, group,
      modificationTime, accessTime, PermissionsMask.fromOctal(permissions), replication, blockSize,
      size)
}

object DirectoryEntry {

  def file(path: Path, metadata: URL, owner: String, group: String,
           modificationTime: Date, accessTime: Date, permissions: PermissionsMask,
           replication: Short, blockSize: Long, size: Long) = DirectoryEntry(
    path, PathType.File, metadata, owner, group, modificationTime, accessTime, permissions,
    replication, blockSize, size)

  def directory(path: Path, metadata: URL, owner: String, group: String, modificationTime: Date,
                accessTime: Date, permissions: PermissionsMask) = DirectoryEntry(
    path, PathType.File, metadata, owner, group, modificationTime, accessTime, permissions,
    DirectoryMetadata.Replication, DirectoryMetadata.BlockSize, DirectoryMetadata.Size)
}
