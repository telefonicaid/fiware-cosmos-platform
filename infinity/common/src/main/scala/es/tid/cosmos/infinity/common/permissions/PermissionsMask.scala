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

package es.tid.cosmos.infinity.common.permissions

/** The UNIX-like permissions of a file. */
case class PermissionsMask(
    owner: PermissionClass,
    group: PermissionClass,
    others: PermissionClass,
    isSticky: Boolean = false) {
  override def toString: String = Seq(if (isSticky) "1" else "", owner, group, others).mkString
}

object PermissionsMask {

  val OctalPattern = """(0|1)?([0-7])([0-7])([0-7])""".r

  /** Obtain the UNIX-like permissions from its octal representation. */
  def fromOctal(octal: String): PermissionsMask = octal match {
    case OctalPattern(s, u, g, o) =>
      PermissionsMask(
        isSticky = s == "1",
        owner = PermissionClass.fromOctal(u),
        group = PermissionClass.fromOctal(g),
        others = PermissionClass.fromOctal(o)
      )
    case _ =>
      throw new IllegalArgumentException(s"invalid octal UNIX permissions in $octal")
  }

  def fromShort(n: Short): PermissionsMask = PermissionsMask(
    isSticky = (n >> 9).toByte == 1,
    owner = PermissionClass.fromByte((n >> 6).toByte),
    group = PermissionClass.fromByte((n >> 3).toByte),
    others = PermissionClass.fromByte(n.toByte)
  )

  /** Check whether given string is a valid octal representation for UNIX permissions. */
  def isValidOctal(expression: String): Boolean = try {
    fromOctal(expression)
    true
  } catch { case _: IllegalArgumentException => false }
}


