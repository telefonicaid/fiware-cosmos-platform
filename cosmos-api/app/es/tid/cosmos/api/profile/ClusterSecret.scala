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
