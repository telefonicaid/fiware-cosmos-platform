package es.tid.cosmos.servicemanager.clusters.sql

import org.squeryl.PrimitiveTypeMode._

import es.tid.cosmos.common.SqlDatabase
import es.tid.cosmos.servicemanager.{ClusterName, ServiceDescription, ClusterUser}
import es.tid.cosmos.servicemanager.clusters._

private[sql] class SqlClusterDao(db: SqlDatabase) extends ClusterDao {
  import ClusterSchema._

  override def getDescription(id: ClusterId): Option[MutableClusterDescription] = newTransaction {
    from(clusterState)(c => where(c.id === id.toString) select c.id)
      .headOption
      .map(_ => new SqlMutableClusterDescription(id, this))
  }

  override def registerCluster(
      id: ClusterId,
      name: ClusterName,
      size: Int,
      services: Set[ServiceDescription]): MutableClusterDescription =
    newTransaction {
      val cluster = clusterState.insert(ClusterEntity(id = id.toString, name = name.toString,
        size = size, nameNode = None, state = Provisioning.name, reason = None))
      services.foreach(service => cluster.services.associate(ClusterServiceEntity(service.name)))
      new SqlMutableClusterDescription(id, this)
    }

  override def ids: Seq[ClusterId] = newTransaction {
    from(clusterState)(select(_)).map(c => ClusterId(c.id)).toSeq
  }

  override def getUsers(clusterId: ClusterId): Option[Set[ClusterUser]] = newTransaction {
    from(clusterState)(c => where(c.id === clusterId.id) select c).headOption.map(
      cluster => cluster.users.map(_.toClusterUser).toSet
    )
  }

  override def setUsers(clusterId: ClusterId, users: Set[ClusterUser]) = newTransaction {
    from(clusterState)(c => where(c.id === clusterId.id) select c).headOption match {
      case Some(cluster) =>
        cluster.users.deleteAll
        users.foreach(usr => cluster.users.associate(ClusterUserEntity(usr)))
      case None => throw new IllegalArgumentException(s"no cluster was found for $clusterId")
    }
  }

  private[sql] def newTransaction[A](a: =>A) = {
    val session = db.newSession
    try {
      transaction(session)(a)
    } finally {
      session.close
    }
  }
}
