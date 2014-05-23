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

import java.util
import java.io.FileNotFoundException
import java.net.URL
import java.util.Date
import scala.util.Try

import org.apache.hadoop.fs.{CreateFlag, FileAlreadyExistsException, ParentNotDirectoryException}
import org.apache.hadoop.hdfs.protocol.{AlreadyBeingCreatedException, DirectoryListing, HdfsFileStatus}
import org.apache.hadoop.hdfs.server.blockmanagement.DatanodeDescriptor
import org.apache.hadoop.hdfs.server.namenode.{NameNode => HadoopNameNode}
import org.apache.hadoop.net.NodeBase
import org.apache.hadoop.security.AccessControlException

import es.tid.cosmos.infinity.common.fs._
import es.tid.cosmos.infinity.common.hadoop.HadoopConversions._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.server.config.MetadataServerConfig
import es.tid.cosmos.infinity.server.urls.UrlMapper

class HdfsNameNode(
    config: MetadataServerConfig,
    hadoopNameNode: HadoopNameNode,
    dfsClientFactory: DfsClientFactory,
    urlMapper: UrlMapper) extends NameNode {

  import HdfsNameNode._

  private val protocols = hadoopNameNode.getRpcServer
  private val nameSystem = hadoopNameNode.getNamesystem

  override def pathMetadata(path: Path): PathMetadata = {
    val fileStatus = checkedFileInfo(path)
    if (fileStatus.isDir) dirMetadataOf(path, fileStatus, directoryContents(path, fileStatus))
    else fileMetadataOf(path, fileStatus)
  }

  override def createFile(
      path: Path,
      owner: String,
      group: String,
      permissions: PermissionsMask,
      replication: Option[Short],
      blockSize: Option[Long]): Unit = forPath(path) {
      dfsClientFactory.withFailsafeClient { client =>
        val out = client.create(
          path.toString,
          permissions.toHadoop,
          util.EnumSet.of(CreateFlag.CREATE),
          DontCreateParent,
          replication.getOrElse(config.replication),
          blockSize.getOrElse(config.blockSize),
          NoProgress,
          DummyBufferSize,
          NoChecksumOptions
        )
        // It's a zero content creation, so nothing to write.
        out.close()
        client.close()
      }
    }

  override def createDirectory(
      path: Path,
      owner: String,
      group: String,
      permissions: PermissionsMask): Unit =
    forPath(path) {
      protocols.mkdirs(
        path.toString,
        permissions.toHadoop,
        DontCreateParent)
    }

  override def deletePath(path: Path, recursive: Boolean): Unit =
    forPath(path) {
      if (!protocols.delete(path.toString, recursive)) {
        throw new IllegalStateException("unexpected return value from NameNode.delete()")
      }
    }

  override def movePath(from: Path, to: Path): Unit = {
    requirePathExists(from)
    requirePathNotExists(to)
    forPath(from) {
      if (!protocols.rename(from.toString, to.toString)) {
        throw new IllegalStateException("unexpected return value from NameNode.rename()")
      }
    }
  }

  override def setOwner(path: Path, newOwner: String): Unit =
    setOwnerAndGroup(path, newOwner, UseSameGroup)

  override def setGroup(path: Path, newGroup: String): Unit =
    setOwnerAndGroup(path, UseSameUser, newGroup)

  override def setPermissions(path: Path, permissions: PermissionsMask): Unit =
    forPath(path) {
      protocols.setPermission(path.toString, permissions.toHadoop)
    }

  private def checkedFileInfo(path: Path): HdfsFileStatus = forPath(path) {
    Option(protocols.getFileInfo(path.toString)).getOrElse(throw NameNodeException.NoSuchPath(path))
  }

  private def directoryContents(path: Path, fileStatus: HdfsFileStatus) = {
    def getListing(start: Array[Byte]) = forPath(path) {
      protocols.getListing(path.toString, start, false)
    }
    lazy val directoryListings: Stream[DirectoryListing] = getListing(HdfsFileStatus.EMPTY_NAME) #::
      directoryListings.takeWhile(_.hasMore).map(prev => getListing(prev.getLastName))
    directoryListings.flatMap(_.getPartialListing).map(s => (path / s.getLocalName, s)).toList
  }

  private def fileMetadataOf(path: Path, fileStatus: HdfsFileStatus) = FileMetadata(
    path,
    metadata = urlMapper.metadataUrl(path),
    content = pickContentServer(path, fileStatus),
    owner = fileStatus.getOwner,
    group = fileStatus.getGroup,
    modificationTime = new Date(fileStatus.getModificationTime),
    accessTime = new Date(fileStatus.getAccessTime),
    permissions = fileStatus.getPermission.toInfinity,
    replication = fileStatus.getReplication,
    blockSize = fileStatus.getBlockSize,
    size = fileStatus.getLen
  )

  private def dirMetadataOf(
      path: Path,
      fileStatus: HdfsFileStatus,
      contents: Seq[(Path, HdfsFileStatus)]) = DirectoryMetadata(
    path,
    metadata = urlMapper.metadataUrl(path),
    owner = fileStatus.getOwner,
    group = fileStatus.getGroup,
    modificationTime = new Date(fileStatus.getModificationTime),
    accessTime = new Date(fileStatus.getAccessTime),
    permissions = fileStatus.getPermission.toInfinity,
    content = contents.map(c => directoryEntryOf(c._1, c._2))
  )

  private def directoryEntryOf(path: Path, fileStatus: HdfsFileStatus) = {
    DirectoryEntry(
      path = path,
      `type` = if (fileStatus.isDir) PathType.Directory else PathType.File,
      metadata = urlMapper.metadataUrl(path),
      owner = fileStatus.getOwner,
      group = fileStatus.getGroup,
      modificationTime = new Date(fileStatus.getModificationTime),
      accessTime = new Date(fileStatus.getAccessTime),
      permissions = fileStatus.getPermission.toInfinity,
      replication = fileStatus.getReplication,
      blockSize = fileStatus.getBlockSize,
      size = fileStatus.getLen
    )
  }
  /** Choose a content server at random by mapping a random datanode to its content server url.
    * The content server uses a DFSClient under the covers. The eventual datanode will be
    * picked according to Hadoop's proximity strategy which should resolve to the same datanode
    * if that node contains the required blocks or one close-by to it that does.
    * This does not offer any proximity strategy with regards to the user.
    */
  private def pickContentServer(path: Path, fileStatus: HdfsFileStatus): URL =
    forPath(path) {
      val dataNodeManager = nameSystem.getBlockManager.getDatanodeManager
      val randomDataNode = dataNodeManager
        .getNetworkTopology
        .chooseRandom(NodeBase.ROOT)
        .asInstanceOf[DatanodeDescriptor]
      urlMapper.contentUrl(path, randomDataNode.getHostName)
    }

  private def setOwnerAndGroup(path: Path, owner: String, group: String) = forPath(path) {
    protocols.setOwner(path.toString, owner, group)
  }

  /** Check whether given path exists, throwing NoSuchPath if not. */
  private def requirePathExists(path: Path): Unit = checkedFileInfo(path)

  /** Check whether given path doesn't exist, throwing PathAlreadyExists if not. */
  private def requirePathNotExists(path: Path): Unit = if (Try(checkedFileInfo(path)).isSuccess) {
    throw NameNodeException.PathAlreadyExists(path)
  }
}

object HdfsNameNode {

  private val UseSameUser, UseSameGroup, NoProgress, NoChecksumOptions = null
  private val DontCreateParent = false
  /** Used for file creation where no content will be written */
  private val DummyBufferSize = 0


  /** Use this to run a block of code related to a path and automatically deal with
    * common exceptions wrapping them in the appropriate [[NameNodeException]].
    *
    * @param path  the path
    * @param block the block of code to execute
    * @tparam T    the type of result the block will return
    * @return      the block's result
    */
  private def forPath[T](path: Path)(block: => T): T = try {
    block
  } catch {
    case e: AccessControlException => throw NameNodeException.Unauthorized(path, e)
    case e: AlreadyBeingCreatedException => throw NameNodeException.PathAlreadyExists(path, e)
    case e: FileNotFoundException => throw NameNodeException.NoSuchPath(path, e)
    case e: FileAlreadyExistsException => throw NameNodeException.PathAlreadyExists(path, e)
    case e: ParentNotDirectoryException => throw NameNodeException.ParentNotDirectory(path, e)
    case e: NameNodeException => throw e
    case e: Throwable => throw NameNodeException.IOError(e)
  }
}
