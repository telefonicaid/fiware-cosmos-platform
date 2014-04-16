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

import java.util
import scala.collection.JavaConverters._

import org.apache.hadoop.conf.{Configurable, Configuration}
import org.apache.hadoop.security.GroupMappingServiceProvider

private[groups] abstract class GroupMappingProvider
  extends GroupMappingServiceProvider with Configurable {

  private var config: Option[Configuration] = None
  private var groupMapping: Option[GroupMapping] = None

  override def setConf(newConfig: Configuration): Unit = {
    config = Some(newConfig)
    groupMapping = Some(buildGroupMapping(newConfig))
  }

  override def getConf: Configuration = config.getOrElse(reportNotInitialized())

  override def cacheGroupsAdd(groups: util.List[String]): Unit = {
    // No caching
  }

  override def cacheGroupsRefresh(): Unit = {
    // No caching
  }

  /** Get all various group memberships of a given user.
    * Returns EMPTY list in case of non-existing user
    *
    * @param handle User's name
    * @return group memberships of user
    */
  override def getGroups(handle: String): util.List[String] =
    groupMapping.getOrElse(reportNotInitialized()).groupsFor(handle).asJava

  /** Build an actual group mapping given a Hadoop Configuration.
    *
    * @throws IllegalArgumentException If configuration is invalid or insufficient
    */
  protected def buildGroupMapping(config: Configuration): GroupMapping

  private def reportNotInitialized() =
    throw new IllegalStateException("Group mapping provider was not yet initialized")
}
