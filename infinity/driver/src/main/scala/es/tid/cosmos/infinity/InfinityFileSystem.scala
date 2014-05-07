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

import java.io.FileNotFoundException
import java.net.{URI, URL}
import java.util.concurrent.TimeoutException
import scala.concurrent.{Await, Future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.util.{Failure, Success, Try}

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
      case RootPath => Future.successful(())
      case subPath @ SubPath(parent, _) => unless(pathExists(subPath)) {
        makeRecursiveDirectory(parent, mask).flatMap(_ => client.createDirectory(subPath, mask))
      }
    }

  private def pathExists(path: InfinityPath): Future[Boolean] = path match {
    case RootPath => Future.successful(true)
    case subPath: SubPath => client.pathMetadata(subPath).map(_.isDefined)
  }

  override def getFileStatus(f: Path): FileStatus =
    awaitResult(client.pathMetadata(f.toInfinity).map {
      case None => throw new FileNotFoundException(f.toString)
      case Some(metadata: FileMetadata) => metadata.toHadoop
      case Some(metadata: DirectoryMetadata) => metadata.toHadoop
    })

  private def unless(condition: Future[Boolean])(body: Future[Unit]): Future[Unit] =
    condition.flatMap(if (_) Future.successful(()) else body)

  /** Blocks for an action completion.
    *
    * @param action  Action to block for
    * @return        Whether the action succeeded
    */
  private def awaitAction(action: Future[_]): Boolean = boundedWait(action) match {
    case Success(_) => true
    case Failure(ex) =>
      Log.error("Cannot perform Infinity file system action", ex)
      false
  }

  /** Blocks for a result to be ready.
    *
    * @param result  Result to wait for
    * @return        The result if the future succeeds on time. Otherwise an exception will
    *                be thrown.
    */
  private def awaitResult[T](result: Future[T]): T = boundedWait(result) match {
    case Success(value) => value
    case Failure(ex) =>
      Log.error("Cannot perform Infinity file system action", ex)
      throw ex;
  }

  /** Blocks for a result to be ready (or failed) */
  private def boundedWait[T](result: Future[T]): Try[T] = try {
    Await.ready(result, infinityConfiguration.timeoutDuration).value.get
  } catch {
    case ex: TimeoutException => Failure(ex)
  }

  override val getScheme = Scheme

  // TODO: Not implemented methods


  override def listStatus(f: Path): Array[FileStatus] = ???

  override def delete(f: Path, recursive: Boolean): Boolean = ???

  override def rename(src: Path, dst: Path): Boolean = ???

  override def append(f: Path, bufferSize: Int, progress: Progressable): FSDataOutputStream = ???

  override def create(f: Path, permission: FsPermission, overwrite: Boolean, bufferSize: Int, replication: Short, blockSize: Long, progress: Progressable): FSDataOutputStream = ???

  override def open(f: Path, bufferSize: Int): FSDataInputStream = ???

  override def setOwner(p: Path, username: String, groupname: String): Unit = super.setOwner(p, username, groupname)

  override def setPermission(p: Path, permission: FsPermission): Unit = super.setPermission(p, permission)

  override def getStatus(p: Path): FsStatus = super.getStatus(p)

  override def getDefaultReplication(path: Path): Short = super.getDefaultReplication(path)

  override def getDefaultBlockSize(f: Path): Long = super.getDefaultBlockSize(f)

  override def concat(trg: Path, psrcs: Array[Path]): Unit = super.concat(trg, psrcs)

  override def append(f: Path, bufferSize: Int): FSDataOutputStream = super.append(f, bufferSize)

  override def append(f: Path): FSDataOutputStream = super.append(f)

  override def createNewFile(f: Path): Boolean = super.createNewFile(f)
}

object InfinityFileSystem {
  val Scheme = "infinity"

  private val Log = LogFactory.getLog(classOf[InfinityFileSystem])

  private object DefaultClientFactory extends InfinityClientFactory{
    override def build(metadataEndpoint: URL) = new HttpInfinityClient(metadataEndpoint)
  }
}
