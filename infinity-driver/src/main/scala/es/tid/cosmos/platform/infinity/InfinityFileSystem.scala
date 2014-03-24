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

package es.tid.cosmos.platform.infinity

import java.net.URI

import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.fs._
import org.apache.hadoop.fs.permission.FsPermission
import org.apache.hadoop.hdfs.DistributedFileSystem
import org.apache.hadoop.util.Progressable

/** Wraps a filesystem (HDFS by default) to change the scheme to infinity://
  *
  * It just translates from the infinity to hdfs schemes ("transform") and back ("transform back").
  */
class InfinityFileSystem(fs: FileSystem) extends FileSystem {

  /** Explicit default constructor that Hadoop looks for */
  def this() = this(new DistributedFileSystem())

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
