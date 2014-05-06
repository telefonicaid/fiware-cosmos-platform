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

package es.tid.cosmos.infinity.common.fs

/** A representation of filesystem path in Infinity. */
sealed trait Path {

  /** The parent directory of this path, or none in case or root path. */
  val parent: Option[Path]

  override def equals(what: Any) = what match {
    case p: Path => p.toString == toString
    case _ => false
  }

  /** Retrieve a child path with this as parent. */
  def / (name: String): SubPath = new SubPath(parentPath = this, name)
}

/** The path corresponding to the root of Infinity filesystem. */
object RootPath extends Path {

  override val parent = None
  override val toString = "/"
}

/** A subpath that have a parent and an element name. */
case class SubPath(parentPath: Path, name: String) extends Path {

  Path.requireValidPathElement(name)

  override val parent = Some(parentPath)

  override def toString: String =  parentPath match {
    case RootPath =>  s"$parentPath$name"
    case _ =>  s"$parentPath${Path.Separator}$name"
  }
}

object Path {

  /** The default separator used in string-to-path conversions. */
  val Separator: Char = '/'

  def requireValidPathElement(name: String): Unit =
    require(!name.contains(Separator),
      s"invalid path element $name (it cannot contain ${Path.Separator} symbol)")

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

  private def pathElements(path: String, separator: Char = Separator): Seq[String] = {
    val stripped = path.stripPrefix(separator.toString).stripSuffix(separator.toString)
    if (stripped.isEmpty) Seq.empty else stripped.split(separator)
  }
}
