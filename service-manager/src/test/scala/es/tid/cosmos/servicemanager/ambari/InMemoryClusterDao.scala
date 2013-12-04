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
import es.tid.cosmos.servicemanager.ClusterUser

class InMemoryClusterDao extends ClusterDao {
  @volatile var clusters: Map[ClusterId, MutableClusterDescription] = Map.empty
  @volatile var users: Map[ClusterId, Set[ClusterUser]] = Map.empty

  override def ids: Seq[ClusterId] = clusters.keys.toSeq

  override def getDescription(id: ClusterId): Option[MutableClusterDescription] = clusters.get(id)

  override def registerCluster(
      clusterId: ClusterId,
      clusterName: String,
      clusterSize: Int): MutableClusterDescription = {
    val description = new InMemoryClusterDescription(clusterId, clusterName, clusterSize)
    clusters.synchronized {
      clusters = clusters.updated(clusterId, description)
    }
    description
  }

  override def getUsers(id: ClusterId): Option[Set[ClusterUser]] = users.get(id)

  override def setUsers(id: ClusterId, newUsers: Set[ClusterUser]) = {
    if (clusters.isDefinedAt(id)) {
      users = users + (id -> newUsers)
    } else {
      throw new IllegalArgumentException(s"no cluster was found for $id")
    }
  }
}
