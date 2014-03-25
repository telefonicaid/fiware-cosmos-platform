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

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys._
import es.tid.cosmos.servicemanager.services.InfinityDriver
import es.tid.cosmos.servicemanager.services.InfinityDriver.InfinityDriverParameters

object AmbariInfinityDriver extends AmbariService with FileConfiguration {
  override val service = InfinityDriver
  override val components =
    Seq(ComponentDescription.allNodesComponent("INFINITY_DRIVER").makeClient)

  override def extraProperties(parameters: InfinityDriverParameters) =
    Map(ClusterSecret -> parameters.clusterSecret)
}
