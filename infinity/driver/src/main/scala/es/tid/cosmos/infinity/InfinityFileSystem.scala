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
import java.util.concurrent.TimeoutException
import scala.concurrent.{Await, Future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.util.{Failure, Success, Try}
import scala.util.control.NonFatal

import org.apache.commons.logging.LogFactory
import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.fs._
import org.apache.hadoop.fs.permission.FsPermission
import org.apache.hadoop.security.UserGroupInformation
import org.apache.hadoop.util.Progressable

import es.tid.cosmos.infinity.client.{HttpInfinityClient, InfinityClient}
import es.tid.cosmos.infinity.common.fs.{Path => InfinityPath, _}
import es.tid.cosmos.infinity.common.hadoop.HadoopConversions._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.streams.{InfinityInputStream, InfinityOutputStream}

class InfinityFileSystem(clientFactory: InfinityClientFactory) extends FileSystem {

  import InfinityFileSystem._

  private var infinityConfiguration: InfinityConfiguration = _
  private var uri: URI = _
  private var ugi: UserGroupInformation = _
  @volatile
  private var workingDirectory: Path = _
  private var client: InfinityClient = _

  def this() = this(InfinityFileSystem.DefaultClientFactory)


  override def setConf(conf: Configuration): Unit = {
    super.setConf(conf)
    infinityConfiguration = new InfinityConfiguration(conf)
  }

  override def initialize(name: URI, conf: Configuration): Unit = {
    require(name.getScheme == Scheme, s"Invalid scheme in $name")
    setConf(conf)
    uri = setDefaultAuthority(name)
    initializeClient()
    ugi = UserGroupInformation.getCurrentUser
    workingDirectory = makeQualified(new Path(homeDirectory()))
    super.initialize(uri, conf)
  }

  private def initializeClient(): Unit = {
    val scheme = if (infinityConfiguration.useSsl) "https" else "http"
    val metadataServerEndpoint = UriUtil.replaceScheme(UriUtil.replacePath(uri, ""), scheme)
    client = clientFactory.build(metadataServerEndpoint.toURL)
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

  private def homeDirectory(): String = s"/user/${ugi.getUserName}"

  override def setTimes(p: Path, mtime: Long, atime: Long): Unit = {
    Log.warn("#setTimes has no effect")
  }

  override def setReplication(src: Path, replication: Short): Boolean = {
    Log.warn("#setReplication has no effect")
    true
  }

  override def mkdirs(f: Path, permission: FsPermission): Boolean =
    awaitAction(makeRecursiveDirectory(f.toInfinity, permission.toInfinity))

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
    awaitResult(existingPathMetadata(f).map(_.toHadoop))

  /** List the status for a path.
    *
    *  * For file paths is just the file status in a singleton array
    *  * For directory paths is the directory listing
    *
    * @throws FileNotFoundException  if path doesn't exist
    */
  override def listStatus(f: Path): Array[FileStatus] =
    awaitResult(existingPathMetadata(f).map {
      case metadata: FileMetadata => Array(metadata.toHadoop)
      case metadata: DirectoryMetadata => metadata.content.map(_.toHadoop).toArray
    })

  override def delete(f: Path, recursive: Boolean): Boolean =
    awaitAction(client.delete(asSubPath(f), recursive))

  override def rename(source: Path, target: Path): Boolean =
    (source.toInfinity, target.toInfinity) match {
      case (from: SubPath, to: SubPath) => awaitAction(client.move(from, to))
      case _ =>
        Log.error(s"Cannot move from/to the root path (Moving $source to $target)")
        false
    }

  override def setOwner(f: Path, ownerOrNull: String, groupOrNull: String): Unit = {
    val path = f.toInfinity
    def ownerChange = whenNotNull(ownerOrNull)(client.changeOwner(path, _))
    def groupChange = whenNotNull(groupOrNull)(client.changeGroup(path, _))
    awaitResult(ownerChange.flatMap(_ => groupChange))
  }

  override def setPermission(f: Path, perms: FsPermission): Unit =
    awaitResult(client.changePermissions(f.toInfinity, perms.toInfinity))

  override def open(f: Path, bufferSize: Int) =
    awaitResult(existingFileMetadata(f).map { metadata =>
      requireContentLocation(metadata)
      new FSDataInputStream(new InfinityInputStream(
        client, asSubPath(f), infinityConfiguration.timeoutDuration))
    })

  override def append(f: Path, bufferSize: Int, progressOrNull: Progressable): FSDataOutputStream =
    awaitResult(appendToFile(f, bufferSize, Option(progressOrNull)))

  private def appendToFile(f: Path, bufferSize: Int, progress: Option[Progressable]): Future[FSDataOutputStream] =
    for {
      metadata <- existingFileMetadata(f)
      stream <- client.append(asSubPath(f), bufferSize)
    } yield new FSDataOutputStream(new InfinityOutputStream(stream, progress), statistics)

  override def create(
      f: Path, perms: FsPermission, overwrite: Boolean, bufferSize: Int, replication: Short,
      blockSize: Long, progressOrNull: Progressable): FSDataOutputStream = {
    val fileCreation = client.createFile(asSubPath(f), perms.toInfinity, Some(replication), Some(blockSize))
    awaitResult(fileCreation.flatMap(_ => appendToFile(f, bufferSize, Option(progressOrNull))))
  }

  private def requireContentLocation(metadata: FileMetadata): Unit = metadata.content.getOrElse(
    throw new IOException(s"${metadata.path} has no known content location"))

  private def existingPathMetadata(f: Path): Future[PathMetadata] =
    client.pathMetadata(f.toInfinity).map(_.getOrElse(throw new FileNotFoundException(f.toString)))

  private def existingFileMetadata(f: Path): Future[FileMetadata] = existingPathMetadata(f).map {
    case _: DirectoryMetadata => throw new IOException(s"$f expected to be a file but was a directory")
    case metadata: FileMetadata => metadata
  }

  private def asSubPath(f: Path): SubPath = f.toInfinity match {
    case RootPath => throw new IOException(s"Cannot perform action on the root path")
    case subPath: SubPath => subPath
  }

  private def unless(condition: Future[Boolean])(body: Future[Unit]): Future[Unit] =
    condition.flatMap(if (_) Ok else body)

  private def whenNotNull[T](valueOrNull: T)(body: T => Future[Unit]): Future[Unit] =
    Option(valueOrNull).fold(Ok)(body)

  /** Blocks for an action completion.
    *
    * @param action  Action to block for
    * @return        Whether the action succeeded
    */
  private def awaitAction(action: => Future[_]): Boolean = {
    val task = try {
      action
    } catch {
      case NonFatal(ex) => Future.failed(ex)
    }
    boundedWait(task) match {
      case Success(_) => true
      case Failure(ex) =>
        Log.error("Cannot perform Infinity file system action", ex)
        false
    }
  }

  /** Blocks for a result to be ready.
    *
    * @param result  Result to wait for
    * @return        The result if the future succeeds on time
    * @throws IOException If action fails or takes too much time
    */
  private def awaitResult[T](result: Future[T]): T = boundedWait(result) match {
    case Success(value) => value
    case Failure(ex: IOException) => throw ex
    case Failure(ex) =>
      Log.error("Cannot perform Infinity file system action", ex)
      throw new IOException("Cannot perform Infinity action", ex)
  }

  /** Blocks for a result to be ready (or failed) */
  private def boundedWait[T](result: Future[T]): Try[T] = try {
    Await.ready(result, infinityConfiguration.timeoutDuration).value.get
  } catch {
    case ex: TimeoutException => Failure(ex)
  }

  override val getScheme = Scheme
}

object InfinityFileSystem {
  val Scheme = "infinity"

  private val Log = LogFactory.getLog(classOf[InfinityFileSystem])

  private object DefaultClientFactory extends InfinityClientFactory{
    override def build(metadataEndpoint: URL) = new HttpInfinityClient(metadataEndpoint)
  }

  private val Ok = Future.successful(())
}
