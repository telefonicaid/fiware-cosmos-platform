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

package es.tid.cosmos.servicemanager.ambari

import java.net.URI

import es.tid.cosmos.servicemanager.clusters._

class InMemoryClusterDao extends ClusterDao {
  @volatile var clusters: Map[ClusterId, MutableClusterDescription] = Map()

  def ids: Seq[ClusterId] = clusters.keys.toSeq

  def getDescription(id: ClusterId): Option[MutableClusterDescription] = clusters.get(id)

  def registerCluster(
      clusterId: ClusterId,
      clusterName: String,
      clusterSize: Int): MutableClusterDescription = {
    val description = new InMemoryClusterDescription(clusterId, clusterName, clusterSize)
    clusters.synchronized {
      clusters = clusters.updated(clusterId, description)
    }
    description
  }
}
