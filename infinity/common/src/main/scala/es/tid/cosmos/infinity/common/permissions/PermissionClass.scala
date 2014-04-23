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

/** A permission class (owner, group, others). */
case class PermissionClass(read: Boolean, write: Boolean, execute: Boolean) {
  override def toString: String = {
    val bin = (if (read) 0x4 else 0) | (if (write) 0x2 else 0) | (if (execute) 0x1 else 0)
    bin.toString
  }
}

object PermissionClass {

  val OctalPattern = """([0-7])""".r

  /** Obtain the permission class from its octal representation. */
  def fromOctal(octal: String): PermissionClass = octal match {
    case OctalPattern(_) =>
      val bin = octal.toByte
      PermissionClass(
        read = (bin & 0x4) == 0x4,
        write = (bin & 0x2) == 0x2,
        execute = (bin & 0x1) == 0x1
      )
    case _ =>
      throw new IllegalArgumentException(s"invalid octal UNIX class permissions in $octal")
  }
}

