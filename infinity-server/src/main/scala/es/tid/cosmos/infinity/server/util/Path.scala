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

package es.tid.cosmos.infinity.server.util

/** A representation of filesystem path in Infinity. */
sealed trait Path {

  /** The parent directory of this path, or none in case or root path. */
  val parent: Option[Path]

  override def equals(what: Any) = what match {
    case p: Path => p.toString == toString
  }

  /** Retrieve a child path with this as parent. */
  def / (name: String): Path = new SubPath(parentPath = this, name)
}

/** The path corresponding to the root of Infinity filesystem. */
object RootPath extends Path {

  override val parent = None
  override val toString = "/"
}

/** A subpath that have a parent and an element name. */
final class SubPath(parentPath: Path, name: String) extends Path {

  require(!name.contains(Path.Separator),
    s"invalid path element $name (it cannot contain ${Path.Separator} symbol)")

  override val parent = Some(parentPath)

  override def toString: String =  parentPath match {
    case RootPath =>  s"$parentPath$name"
    case _ =>  s"$parentPath${Path.Separator}$name"
  }
}

object Path {

  /** The default separator used in string-to-path conversions. */
  val Separator: Char = '/'

  /** Create a new path from given parent and name. */
  def apply(parent: Path, name: String): Path = new SubPath(parent, name)

  /** Retrieve the home directory for the given user. */
  def userDirectory(username: String): Path = {
    require(username.nonEmpty, s"user directory needs a non-empty username")
    RootPath / "user" / username
  }

  /** Convert the given string into an absolute path. */
  def absolute(path: String): Path = {
    require(path.isEmpty || path.head == Separator,
      s"Path $path is not absolute (leading $Separator missing)")
    absolute(pathElements(path, Separator))
  }

  /** Convert the given sequence of strings into an absolute path. */
  def absolute(path: Seq[String]): Path = path match {
    case Seq() => RootPath
    case _ => new SubPath(absolute(path.init), path.last)
  }

  /** Convert the given string into a relative path. */
  def relative(path: String, username: String): Path =
    relative(pathElements(path, Separator), username)

  /** Convert the given sequence of string into a relative path. */
  def relative(path: Seq[String], username: String): Path =
    path.foldLeft(userDirectory(username)) { (p, n) =>
      p / n
    }

  private def pathElements(path: String, separator: Char): Seq[String] = {
    val stripped = path.stripPrefix(separator.toString).stripSuffix(separator.toString)
    if (stripped.isEmpty) Seq.empty else stripped.split(separator)
  }
}
