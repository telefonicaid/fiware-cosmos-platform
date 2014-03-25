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

/** It translates from the infinity scheme to another scheme ("transform") and back ("transform back").
  *
  * @constructor
  * @param defaultAuthority   Authority (host and port) of the default Infinity instance
  */
class UrlMapper(defaultAuthority: Option[String], targetScheme: String = "webhdfs") {
  import UrlMapper._

  def uri = new URI(s"$InfinityScheme:///")

  def transform(name: URI): URI = prependUserPath(setDefaultAuthority(setTargetScheme(name)))

  def transform(path: Path): Path = new Path(transform(path.toUri))

  def transformBack(name: URI): URI = removeUserPath(
    UriUtil.replaceAuthority(UriUtil.replaceScheme(name, InfinityScheme), null))

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

  private def setTargetScheme(name: URI) = UriUtil.replaceScheme(name, targetScheme)

  private def setDefaultAuthority(name: URI) = {
    val currentAuthority = name.getAuthority match {
      case null => None
      case "" => None
      case auth => Some(auth)
    }
    val newAuthority = currentAuthority orElse defaultAuthority getOrElse ""
    UriUtil.replaceAuthority(name, newAuthority)
  }

  private def prependUserPath(name: URI) =
    UriUtil.replacePath(name, "/user" + name.getPath)

  private def removeUserPath(name: URI) = {
    val prefix = "/user"
    val oldPath = name.getPath
    require(oldPath.startsWith(prefix), s"URI ${name.toString} does not have an infinity mapping")
    UriUtil.replacePath(name, oldPath.substring(prefix.length))
  }
}

object UrlMapper {
  val InfinityScheme = "infinity"
}
