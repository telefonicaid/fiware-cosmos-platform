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

package es.tid.cosmos.common

/** Utility to format/extract a token from/to a token bearer header. */
object BearerToken {

  private val BearerPattern = """\s*(?i)bearer(?-i)\s+(.+?)\s*""".r

  def apply(token: String): String = s"Bearer $token"

  def unapply(header: String): Option[String] = header match {
    case BearerPattern(token) => Some(token)
    case _ => None
  }
}
