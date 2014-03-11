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

package es.tid.cosmos.api.profile

import java.security.SecureRandom

/** Cluster secrets are tokens of fixed length that grant access to services when used
  * from their associated cluster.
  */
case class ClusterSecret(underlying: String) {
  require(underlying.matches(ClusterSecret.pattern), s"Malformed cluster secret: '$underlying'")

  /** Shows only the last digits to avoid unintended leakage of the cluster secret */
  override def toString = "****" + underlying.takeRight(4)
}

object ClusterSecret {

  val Length: Int = 128

  def random(): ClusterSecret = ClusterSecret(Seq.fill(Length)(randomChar()).mkString)

  private def randomChar() = characters.charAt(generator.nextInt(characters.length))

  private val generator = new SecureRandom()
  private val characters = "01234567890abcdefghijklmnopqrstuvwzABCDEFGHIJKLMNOPQRSTUVWZ"
  private val pattern = s"[a-zA-Z0-9]{$Length}"
}
