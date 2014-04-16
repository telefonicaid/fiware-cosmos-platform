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

import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys._
import es.tid.cosmos.servicemanager.services.InfinityServer
import es.tid.cosmos.servicemanager.services.InfinityServer.InfinityServerParameters

object AmbariInfinityServer extends AmbariService with FileConfiguration {
  override val service = InfinityServer
  override val components: Seq[ComponentDescription] =
    Seq(ComponentDescription.allNodesComponent("INFINITY_SERVER").makeClient)

  override def extraProperties(parameters: InfinityServerParameters): ConfigProperties = Map(
    CosmosApiUrl -> parameters.cosmosApiUrl,
    InfinitySecret -> parameters.infinitySecret
  )
}
