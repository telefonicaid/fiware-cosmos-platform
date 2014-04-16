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

package es.tid.cosmos.infinity.server.permissions

/** The UNIX-like permissions of a file. */
case class PermissionsMask(owner: PermissionClass, group: PermissionClass, others: PermissionClass) {
  override def toString: String = owner.toString + group.toString + others.toString
}

object PermissionsMask {

  val OctalPattern = """([0-7])([0-7])([0-7])""".r

  /** Obtain the UNIX-like permissions from its octal representation. */
  def fromOctal(octal: String): PermissionsMask = octal match {
    case OctalPattern(u, g, o) =>
      PermissionsMask(
        owner = PermissionClass.fromOctal(u),
        group = PermissionClass.fromOctal(g),
        others = PermissionClass.fromOctal(o)
      )
    case _ =>
      throw new IllegalArgumentException(s"invalid octal UNIX permissions in $octal")
  }

  /** Check whether given string is a valid octal representation for UNIX permissions. */
  def isValidOctal(expression: String): Boolean = try {
    fromOctal(expression)
    true
  } catch { case _: IllegalArgumentException => false }
}


