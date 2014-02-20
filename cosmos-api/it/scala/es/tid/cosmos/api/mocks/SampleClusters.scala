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

package es.tid.cosmos.api.mocks

import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager.ClusterProperties
import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}
import es.tid.cosmos.servicemanager.clusters.{Running, ClusterId}

trait SampleClusters {
  self: WithTestApplication =>

  import SampleClusters._

  mockedServiceManager.defineCluster(RunningClusterProps)
  mockedServiceManager.defineCluster(ProvisioningClusterProps)
}

object SampleClusters {

  val RunningClusterProps = ClusterProperties(
    id = new ClusterId(),
    name = ClusterName("Running cluster"),
    size = 4,
    users = Set(ClusterUser.enabled("jsmith", "jsmith-public-key")),
    initialState = Some(Running)
  )

  val ProvisioningClusterProps = ClusterProperties(
    id = new ClusterId(),
    name = ClusterName("Provisioning cluster"),
    size = 4,
    users = Set(ClusterUser.enabled("pocahontas", "pocahontas-public-key")),
    initialState = None
  )
}
