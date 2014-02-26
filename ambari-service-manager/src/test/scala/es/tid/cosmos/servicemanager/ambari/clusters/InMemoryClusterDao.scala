package es.tid.cosmos.servicemanager.ambari.clusters

import org.scalatest.Assertions
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser, ServiceDescription}
import es.tid.cosmos.servicemanager.clusters.{MutableClusterDescription, ClusterId, ClusterDao}

class InMemoryClusterDao extends ClusterDao with MustMatchers with Assertions {

  @volatile
  private var clusters = Map.empty[ClusterId, InMemoryClusterDescription]

  override def registerCluster(
      id: ClusterId, name: ClusterName, size: Int, services: Set[ServiceDescription]) = synchronized {
    clusters.keySet must not contain id
    val newCluster = new InMemoryClusterDescription(id, name, size, services.map(_.name))
    clusters += id -> newCluster
    newCluster
  }

  override def setUsers(id: ClusterId, users: Set[ClusterUser]) {
    clusters.keySet must contain (id)
    clusters(id).users = users
  }

  override def getUsers(id: ClusterId): Option[Set[ClusterUser]] = clusters.get(id).flatMap(_.users)

  override def getDescription(id: ClusterId): Option[MutableClusterDescription] = clusters.get(id)

  override def ids: Seq[ClusterId] = clusters.keys.toSeq
}
