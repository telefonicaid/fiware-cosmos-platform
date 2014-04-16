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

import java.security.SecureRandom

/** Utilities for secure random token generation of alphanumeric tokens of a given length. */
class AlphanumericTokenPattern(name: String, length: Int) {
  require(length > 0, "Token patterns must have a strictly positive length")

  /** Generates a random token. */
  def generateRandom(): String = Seq.fill(length)(randomChar()).mkString

  def requireValid(token: String): Unit =
    require(token.matches(pattern), s"$name must be an alphanumeric sequence of length $length")

  private def randomChar() = characters.charAt(generator.nextInt(characters.length))

  private val generator = new SecureRandom()
  private val characters = "01234567890abcdefghijklmnopqrstuvwzABCDEFGHIJKLMNOPQRSTUVWZ"
  private val pattern = s"[a-zA-Z0-9]{$length}"
}
