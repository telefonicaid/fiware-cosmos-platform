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

package es.tid.cosmos.platform.infinity

import java.net.URI

import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.fs._
import org.apache.hadoop.fs.permission.FsPermission
import org.apache.hadoop.hdfs.web.SecureWebHdfsFS
import org.apache.hadoop.util.Progressable

/** Wraps a filesystem (SecureWebHDFS by default) to change the scheme to infinity://
  *
  * It just translates from the infinity to hdfs schemes ("transform") and back ("transform back").
  */
class InfinityFileSystem(fs: FileSystem) extends FileSystem {

  /** Explicit default constructor that Hadoop looks for */
  def this() = this(new SecureWebHdfsFS)

  import InfinityFileSystem._

  private var mapper: UrlMapper = null

  override def initialize(name: URI, conf: Configuration) {
    require(name.getScheme == UrlMapper.InfinityScheme, s"Invalid scheme in name $name")
    setConf(conf)
    val authority = Option(name.getAuthority).orElse(Option(getConf.get(DefaultAuthorityProperty)))
    mapper = new UrlMapper(authority)
    super.initialize(name, conf)
    fs.initialize(mapper.transform(name), conf)
  }

  override def getUri: URI = mapper.uri

  override def open(f: Path, bufferSize: Int) = fs.open(mapper.transform(f), bufferSize)

  override def create(
      f: Path, permission: FsPermission, overwrite: Boolean, bufferSize: Int,
      replication: Short, blockSize: Long, progress: Progressable) = fs.create(
    mapper.transform(f), permission, overwrite, bufferSize, replication, blockSize, progress
  )

  override def append(f: Path, bufferSize: Int, progress: Progressable) =
    fs.append(mapper.transform(f), bufferSize, progress)

  override def rename(src: Path, dst: Path) = fs.rename(mapper.transform(src), mapper.transform(dst))

  override def delete(f: Path, recursive: Boolean) = fs.delete(mapper.transform(f), recursive)

  override def listStatus(f: Path): Array[FileStatus] =
    fs.listStatus(mapper.transform(f)).map(status => mapper.transformBack(status))

  override def setWorkingDirectory(newDir: Path) {
    fs.setWorkingDirectory(mapper.transform(newDir))
  }

  override def getWorkingDirectory = mapper.transformBack(fs.getWorkingDirectory)

  override def getHomeDirectory: Path = makeQualified(new Path(System.getProperty("user.name")))

  override def mkdirs(f: Path, permission: FsPermission) = fs.mkdirs(mapper.transform(f), permission)

  override def getFileStatus(f: Path) = mapper.transformBack(fs.getFileStatus(mapper.transform(f)))

  override def getScheme() = UrlMapper.InfinityScheme
}

object InfinityFileSystem {
  val DefaultAuthorityProperty: String = "fs.infinity.defaultAuthority"
}
