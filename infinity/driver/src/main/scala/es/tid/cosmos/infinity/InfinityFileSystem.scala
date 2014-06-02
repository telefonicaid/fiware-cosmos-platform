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

package es.tid.cosmos.infinity

import java.io.{FileSystem => _, _}
import java.net.{URI, URL}
import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration.FiniteDuration

import org.apache.commons.logging.LogFactory
import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.fs._
import org.apache.hadoop.fs.permission.FsPermission
import org.apache.hadoop.security.UserGroupInformation
import org.apache.hadoop.util.Progressable

import es.tid.cosmos.infinity.client.{AlreadyExistsException, HttpInfinityClient, InfinityClient}
import es.tid.cosmos.infinity.common.credentials.Credentials
import es.tid.cosmos.infinity.common.fs.{Path => InfinityPath, _}
import es.tid.cosmos.infinity.common.hadoop.HadoopConversions._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.common.util.{TimeBound, UriUtil}
import es.tid.cosmos.infinity.streams.{InfinityInputStream, InfinityOutputStream}

class InfinityFileSystem(clientFactory: InfinityClientFactory) extends FileSystem {

  import InfinityFileSystem._

  private var infinityConfiguration: InfinityConfiguration = _
  private var uri: URI = _
  private var ugi: UserGroupInformation = _
  @volatile
  private var workingDirectory: Path = _
  private var client: InfinityClient = _
  private var shortTimeBound: TimeBound = _

  def this() = this(InfinityFileSystem.DefaultClientFactory)


  override def setConf(conf: Configuration): Unit = {
    super.setConf(conf)
    infinityConfiguration = new InfinityConfiguration(conf)
  }

  override def initialize(name: URI, conf: Configuration): Unit = {
    require(name.getScheme == Scheme, s"Invalid scheme in $name")
    setConf(conf)
    uri = setDefaultAuthority(name)
    shortTimeBound = new TimeBound(infinityConfiguration.shortOperationTimeout)
    initializeClient()
    ugi = UserGroupInformation.getCurrentUser
    workingDirectory = makeQualified(new Path(homeDirectory()))
    super.initialize(uri, conf)
  }

  private def initializeClient(): Unit = {
    val scheme = if (infinityConfiguration.useSsl) "https" else "http"
    val metadataServerEndpoint = UriUtil.replaceScheme(UriUtil.replacePath(uri, ""), scheme)
    val credentials = infinityConfiguration.credentials
      .getOrElse(throw new IllegalArgumentException("No credentials were configured"))
    client = clientFactory.build(
      metadataServerEndpoint.toURL,
      credentials,
      infinityConfiguration.longOperationTimeout)
  }

  /** If authority is missing, add the default one. */
  private def setDefaultAuthority(name: URI): URI =
    if (name.getAuthority != null) name
    else UriUtil.replaceAuthority(name, infinityConfiguration.defaultAuthority.orNull)

  override def getUri: URI = uri

  override def getWorkingDirectory: Path = workingDirectory

  override def setWorkingDirectory(newDir: Path): Unit = {
    workingDirectory = makeAbsolute(newDir)
  }

  private def makeAbsolute(path: Path): Path = new Path(getHomeDirectory, path)

  override def getHomeDirectory: Path = makeQualified(new Path(homeDirectory()))

  private def homeDirectory(): String = s"/${ugi.getUserName}"

  override def setTimes(p: Path, mtime: Long, atime: Long): Unit = {
    Log.warn("#setTimes has no effect")
  }

  override def setReplication(src: Path, replication: Short): Boolean = {
    Log.warn("#setReplication has no effect")
    true
  }

  override def mkdirs(f: Path): Boolean = mkdirs(f, defaultDirectoryPermissions)

  override def mkdirs(f: Path, permission: FsPermission): Boolean =
    shortTimeBound.awaitAction(makeRecursiveDirectory(absolutePath(f), permission.toInfinity))

  override protected def canonicalizeUri(uri: URI): URI =
    new URI(uri.getScheme, null, uri.getPath, uri.getQuery, uri.getFragment)

  private def makeRecursiveDirectory(path: InfinityPath, mask: PermissionsMask): Future[Unit] =
    path match {
      case RootPath => Ok
      case subPath @ SubPath(parent, _) => unless(pathExists(subPath)) {
        makeRecursiveDirectory(parent, mask).flatMap(_ => client.createDirectory(subPath, mask))
      }
    }

  private def pathExists(path: InfinityPath): Future[Boolean] = path match {
    case RootPath => Future.successful(true)
    case subPath: SubPath => client.pathMetadata(subPath).map(_.isDefined)
  }

  override def getFileStatus(f: Path): FileStatus =
    shortTimeBound.awaitResult(existingPathMetadata(f).map(transformBack(_).toHadoop))

  /** List the status for a path.
    *
    *  * For file paths is just the file status in a singleton array
    *  * For directory paths is the directory listing
    *
    * @throws FileNotFoundException  if path doesn't exist
    */
  override def listStatus(f: Path): Array[FileStatus] =
    shortTimeBound.awaitResult(existingPathMetadata(f).map {
      case metadata: FileMetadata => Array(transformBack(metadata).toHadoop)
      case metadata: DirectoryMetadata => metadata.content.map(transformBack(_).toHadoop).toArray
    })

  private def transformBack(metadata: PathMetadata): PathMetadata = metadata match {
    case file: FileMetadata => file.copy(path = UserPathMapper.userPath(file.path))
    case dir: DirectoryMetadata => dir.copy(
      path = UserPathMapper.userPath(dir.path),
      content = dir.content.map(transformBack)
    )
  }

  private def transformBack(entry: DirectoryEntry): DirectoryEntry =
    entry.copy(path = UserPathMapper.userPath(entry.path))

  override def delete(f: Path, recursive: Boolean): Boolean =
    if (f.isRoot) false else shortTimeBound.awaitAction(client.delete(absolutePath(f), recursive))

  override def rename(source: Path, target: Path): Boolean =
    if (source.isRoot || target.isRoot) {
      Log.error(s"Cannot move from/to the root path (Moving $source to $target)")
      false
    } else shortTimeBound.awaitAction(client.move(absolutePath(source), absolutePath(target)))

  override def setOwner(f: Path, ownerOrNull: String, groupOrNull: String): Unit = {
    val path = absolutePath(f)
    def ownerChange = whenNotNull(ownerOrNull)(client.changeOwner(path, _))
    def groupChange = whenNotNull(groupOrNull)(client.changeGroup(path, _))
    shortTimeBound.awaitResult(ownerChange.flatMap(_ => groupChange))
  }

  override def setPermission(f: Path, perms: FsPermission): Unit =
    shortTimeBound.awaitResult(client.changePermissions(absolutePath(f), perms.toInfinity))

  override def open(f: Path, bufferSize: Int) =
    shortTimeBound.awaitResult(existingFileMetadata(f).map { metadata =>
      new FSDataInputStream(new InfinityInputStream(
        client, absolutePath(f), bufferSize, infinityConfiguration.shortOperationTimeout
      ))
    })

  override def append(f: Path, bufferSize: Int, progressOrNull: Progressable): FSDataOutputStream =
    shortTimeBound.awaitResult(appendToFile(f, bufferSize, Option(progressOrNull)))

  private def changeFileContent(changeFileFunction: (SubPath, Int) => Future[OutputStream])(
      f: Path,
      bufferSize: Int,
      progress: Option[Progressable]): Future[FSDataOutputStream] =
    for {
      metadata <- existingFileMetadata(f)
      stream <- changeFileFunction(absolutePath(f), bufferSize)
    } yield new FSDataOutputStream(new InfinityOutputStream(stream, progress), statistics)

  private def overwriteFile = changeFileContent(client.overwrite) _

  private def appendToFile = changeFileContent(client.append) _

  override def create(
      f: Path, perms: FsPermission, overwrite: Boolean, bufferSize: Int, replication: Short,
      blockSize: Long, progressOrNull: Progressable): FSDataOutputStream =
    if (f.isRoot) throw new IOException("Cannot create the root directory")
    else {
      val absPath = absolutePath(f)
      val fileWriteFunction = if (overwrite) overwriteFile else appendToFile
      val fileWriteStream_> = for {
        _ <- makeRecursiveDirectory(absolutePath(f.getParent), defaultDirectoryPermissions.toInfinity)
        _ <- client.createFile(absPath, perms.toInfinity, Some(replication), Some(blockSize)).recover {
          case _: AlreadyExistsException if overwrite => ()
        }
        stream <- fileWriteFunction(f, bufferSize, Option(progressOrNull))
      } yield stream
      shortTimeBound.awaitResult(fileWriteStream_>)
    }

  private def existingPathMetadata(f: Path): Future[PathMetadata] =
    client.pathMetadata(absolutePath(f)).map(_.getOrElse(throw new FileNotFoundException(f.toString)))

  private def existingFileMetadata(f: Path): Future[FileMetadata] = existingPathMetadata(f).map {
    case _: DirectoryMetadata => throw new IOException(s"$f expected to be a file but was a directory")
    case metadata: FileMetadata => metadata
  }

  private def absolutePath(f: Path): SubPath = UserPathMapper.absolutePath(f.toInfinity)

  private def unless(condition: Future[Boolean])(body: => Future[Unit]): Future[Unit] =
    condition.flatMap(if (_) Ok else body)

  private def whenNotNull[T](valueOrNull: T)(body: T => Future[Unit]): Future[Unit] =
    Option(valueOrNull).fold(Ok)(body)

  private def defaultDirectoryPermissions: FsPermission =
    FsPermission.getDirDefault.applyUMask(infinityConfiguration.umask)

  override val getScheme = Scheme
}

object InfinityFileSystem {
  val Scheme = "infinity"

  private val Log = LogFactory.getLog(classOf[InfinityFileSystem])

  private object DefaultClientFactory extends InfinityClientFactory {
    override def build(
        metadataEndpoint: URL, credentials: Credentials, longOperationTimeout: FiniteDuration) =
      new HttpInfinityClient(metadataEndpoint, credentials, longOperationTimeout)
  }

  private val Ok = Future.successful(())
}
