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

  override def initialize(name: URI, conf: Configuration) {
    require(name.getScheme == InfinityFileSystem.InfinityScheme, s"Invalid scheme in name $name")
    setConf(conf)
    super.initialize(name, conf)
    fs.initialize(transformName(name), conf)
  }

  override def getUri: URI = UriUtil.replaceScheme(fs.getUri, InfinityScheme)

  override def makeQualified(path: Path) = fs.makeQualified(transformPath(path))

  override def open(f: Path, bufferSize: Int) = fs.open(transformPath(f), bufferSize)

  override def create(
      f: Path, permission: FsPermission, overwrite: Boolean, bufferSize: Int,
      replication: Short, blockSize: Long, progress: Progressable) = fs.create(
    transformPath(f), permission, overwrite, bufferSize, replication, blockSize, progress
  )

  override def append(f: Path, bufferSize: Int, progress: Progressable) =
    fs.append(transformPath(f), bufferSize, progress)

  override def rename(src: Path, dst: Path) = fs.rename(transformPath(src), transformPath(dst))

  override def delete(f: Path) = delete(f, recursive = false)

  override def delete(f: Path, recursive: Boolean) = fs.delete(transformPath(f), recursive)

  override def listStatus(f: Path): Array[FileStatus] =
    fs.listStatus(transformPath(f)).map(status => transformBackStatus(status))

  override def setWorkingDirectory(newDir: Path) {
    fs.setWorkingDirectory(transformPath(newDir))
  }

  override def getWorkingDirectory = transformBackPath(fs.getWorkingDirectory)

  override def mkdirs(f: Path, permission: FsPermission) = fs.mkdirs(transformPath(f), permission)

  override def getFileStatus(f: Path) = transformBackStatus(fs.getFileStatus(transformPath(f)))

  private def setHdfsScheme(name: URI) = UriUtil.replaceScheme(name, HdfsScheme)

  private def setDefaultAuthority(name: URI) = {
    val currentAuthority = name.getAuthority match {
      case null => None
      case "" => None
      case auth => Some(auth)
    }
    val defaultAuthority = Option(getConf.get(DefaultAuthorityProperty))
    val newAuthority = currentAuthority orElse defaultAuthority getOrElse ""
    UriUtil.replaceAuthority(name, newAuthority)
  }

  private def transformName(name: URI) = setDefaultAuthority(setHdfsScheme(name))

  private def transformPath(path: Path) = new Path(transformName(path.toUri))

  private def transformBackPath(path: Path) =
    if (path == null) null
    else new Path(UriUtil.replaceScheme(path.toUri, InfinityScheme))

  private def transformBackStatus(s: FileStatus) = new FileStatus(
    s.getLen,
    s.isDir,
    s.getReplication,
    s.getBlockSize,
    s.getModificationTime,
    s.getAccessTime,
    s.getPermission,
    s.getOwner,
    s.getGroup,
    transformBackPath(s.getPath)
  )
}

object InfinityFileSystem {
  val InfinityScheme = "infinity"
  val HdfsScheme = "hdfs"
  val DefaultAuthorityProperty: String = "fs.infinity.defaultAuthority"
}
