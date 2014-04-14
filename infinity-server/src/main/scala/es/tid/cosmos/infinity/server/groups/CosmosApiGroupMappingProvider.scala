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
