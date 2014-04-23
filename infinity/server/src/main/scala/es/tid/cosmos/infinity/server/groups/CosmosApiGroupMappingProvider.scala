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

package es.tid.cosmos.infinity.server.groups

import scala.concurrent.duration._

import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.security.JniBasedUnixGroupsMappingWithFallback

/** Implementation of the username to groups mapping based on querying the Cosmos API.
  * Note that at the moment no caching is performed.
  */
class CosmosApiGroupMappingProvider extends GroupMappingProvider {

  import CosmosApiGroupMappingProvider._

  override protected def buildGroupMapping(config: Configuration): CosmosApiGroupMapping = {

    def requireKey(key: String) = Option(config.get(key)).getOrElse(
      throw new IllegalArgumentException(s"Missing key $key for configuring groups mapping"))

    new CosmosApiGroupMapping(
      apiBase = requireKey(CosmosApiBaseKey),
      infinitySecret = requireKey(InfinitySecretKey),
      apiTimeout = timeout(config),
      fallbackMapping = new JniBasedUnixGroupsMappingWithFallback
    )
  }

  private def timeout(config: Configuration) = config.getInt(CosmosApiTimeoutKey, 0) match {
    case positive if positive > 0 => positive.millis
    case 0 => Duration.Inf
    case negative =>
      throw new IllegalArgumentException(s"$CosmosApiTimeoutKey should not be negative")
  }
}

object CosmosApiGroupMappingProvider {

  /** Prefix for all Hadoop configuration related to group mapping */
  val GroupMappingKeyPrefix = "hadoop.security.group.mapping"

  val InfinityKeyPrefix = GroupMappingKeyPrefix + ".infinity"
  val CosmosApiBaseKey = InfinityKeyPrefix + ".apiBase"
  val CosmosApiTimeoutKey = InfinityKeyPrefix + ".timeout"
  val InfinitySecretKey = InfinityKeyPrefix + ".secret"
}
