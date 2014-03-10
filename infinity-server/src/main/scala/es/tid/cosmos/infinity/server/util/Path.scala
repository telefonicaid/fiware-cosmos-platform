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
class Path(val parent: Option[Path], name: String) {

  require(!name.contains(Path.Separator),
    s"invalid path element $name (it cannot contain ${Path.Separator} symbol)")

  override def equals(what: Any) = what match {
    case p: Path => p.toString == toString
  }

  override def toString: String =  parent match {
    case Some(p) if p.parent.isDefined =>  s"$p${Path.Separator}$name"
    case Some(p) if !p.parent.isDefined =>  s"${Path.Separator}$name"
    case None => "/"
  }

  /** Retrieve a child path with this as parent. */
  def / (name: String): Path = new Path(parent = Some(this), name)
}

object Path {

  /** The default separator used in string-to-path conversions. */
  val Separator: Char = '/'

  /** The root path. */
  val Root = new Path(parent = None, name = "")

  /** A convenience function to generate absolute paths. */
  def / (name: String) = Root / name

  /** Retrieve the home directory for the given user. */
  def userDirectory(username: String): Path = {
    require(username.nonEmpty, s"user directory needs a non-empty username")
    Path / "user" / username
  }

  /** Convert the given string into an absolute path. */
  def absolute(path: String): Path = {
    require(path.isEmpty || path.head == Separator,
      s"Path $path is not absolute (leading $Separator missing)")
    absolute(pathElements(path, Separator))
  }

  /** Convert the given sequence of strings into an absolute path. */
  def absolute(path: Seq[String]): Path = path match {
    case Seq() => Root
    case _ => new Path(Some(absolute(path.dropRight(1))), path.last)
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
