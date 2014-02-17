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

package es.tid.cosmos.servicemanager.ambari.mocks

import es.tid.cosmos.servicemanager.{ServiceDescription, ClusterUser}
import es.tid.cosmos.servicemanager.clusters._

class InMemoryClusterDao extends ClusterDao {
  @volatile var clusters: Map[ClusterId, MutableClusterDescription] = Map.empty
  @volatile var users: Map[ClusterId, Set[ClusterUser]] = Map.empty

  override def ids: Seq[ClusterId] = clusters.keys.toSeq

  override def getDescription(id: ClusterId): Option[MutableClusterDescription] = clusters.get(id)

  override def registerCluster(
      clusterId: ClusterId,
      clusterName: String,
      clusterSize: Int,
      services: Set[ServiceDescription]): MutableClusterDescription = {
    val description = new InMemoryClusterDescription(
      clusterId, clusterName, clusterSize, services.map(_.name))
    clusters.synchronized {
      clusters = clusters.updated(clusterId, description)
    }
    description
  }

  override def getUsers(id: ClusterId): Option[Set[ClusterUser]] = clusters(id).users

  override def setUsers(id: ClusterId, newUsers: Set[ClusterUser]) =
    clusters(id).users = newUsers

}
