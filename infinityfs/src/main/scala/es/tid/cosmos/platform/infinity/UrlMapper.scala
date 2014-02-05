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

import org.apache.hadoop.fs.{FileStatus, Path}

/** It translates from the infinity to hdfs schemes ("transform") and back ("transform back").
  *
  * @constructor
  * @param defaultAuthority   Authority (host and port) of the default Infinity instance
  */
class UrlMapper(defaultAuthority: Option[String]) {
  import UrlMapper._

  def transform(name: URI): URI = setDefaultAuthority(setHdfsScheme(name))

  def transform(path: Path): Path = new Path(transform(path.toUri))

  def transformBack(name: URI): URI = UriUtil.replaceScheme(name, InfinityScheme)

  def transformBack(path: Path): Path = new Path(transformBack(path.toUri))

  def transformBack(s: FileStatus): FileStatus = new FileStatus(
    s.getLen,
    s.isDirectory,
    s.getReplication,
    s.getBlockSize,
    s.getModificationTime,
    s.getAccessTime,
    s.getPermission,
    s.getOwner,
    s.getGroup,
    if (s.isSymlink) transformBack(s.getSymlink) else null,
    transformBack(s.getPath)
  )

  private def setHdfsScheme(name: URI) = UriUtil.replaceScheme(name, HdfsScheme)

  private def setDefaultAuthority(name: URI) = {
    val currentAuthority = name.getAuthority match {
      case null => None
      case "" => None
      case auth => Some(auth)
    }
    val newAuthority = currentAuthority orElse defaultAuthority getOrElse ""
    UriUtil.replaceAuthority(name, newAuthority)
  }
}

object UrlMapper {
  val InfinityScheme = "infinity"
  val HdfsScheme = "hdfs"
}
