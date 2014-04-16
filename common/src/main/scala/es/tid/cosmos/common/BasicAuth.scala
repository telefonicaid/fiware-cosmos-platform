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

import scala.util.Try

import org.apache.commons.codec.binary.Base64

/** Extractor/formatter class for basic HTTP authorization headers. */
object BasicAuth {

  /** Formats a realm and a password as a basic HTTP authorization header. */
  def apply(user: String, password: String): String =
    s"Basic ${Base64.encodeBase64String(s"$user:$password".getBytes)}"

  /** Extracts a tuple of realm and password from a valid HTTP Auth Basic header.
    *
    * @param header String of the form "Basic: base-64-encoded-user-pass"
    * @return       Realm and password or nothing
    */
  def unapply(header: String): Option[(String, String)] =
    if (header.toLowerCase.startsWith("basic ")) decodeCredentials(header.substring(6))
    else None

  private def decodeCredentials(encodedAuthorization: String) =
    decode(encodedAuthorization).map(_.split(':')).collect {
      case Array(user, pass) => (user, pass)
    }

  private def decode(s: String) = Try(Base64.decodeBase64(s)).toOption.map(new String(_))
}
