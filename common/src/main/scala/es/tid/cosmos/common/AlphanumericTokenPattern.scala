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
