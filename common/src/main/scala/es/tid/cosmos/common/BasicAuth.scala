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
