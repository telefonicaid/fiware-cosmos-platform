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

import es.tid.cosmos.common.AlphanumericTokenPattern

/** Cluster secrets are tokens of fixed length that grant access to services when used
  * from their associated cluster.
  */
case class ClusterSecret(underlying: String) {
  ClusterSecret.TokenPattern.requireValid(underlying)

  /** Shows only the last digits to avoid unintended leakage of the cluster secret */
  override def toString = "****" + underlying.takeRight(4)
}

object ClusterSecret {
  val Length: Int = 128
  private val TokenPattern = new AlphanumericTokenPattern("Cluster secret", Length)

  def random(): ClusterSecret = ClusterSecret(TokenPattern.generateRandom())
}
