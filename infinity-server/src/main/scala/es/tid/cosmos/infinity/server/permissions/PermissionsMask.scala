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


