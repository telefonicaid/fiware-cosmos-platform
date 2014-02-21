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

import com.imageworks.migration.{InstallAllMigrations, Migrator, MysqlDatabaseAdapter}
import org.squeryl.{ForeignKeyDeclaration, KeyedEntity, Schema}
import org.squeryl.PrimitiveTypeMode._
import org.squeryl.annotations.Column
import org.squeryl.dsl.{ManyToOne, OneToMany, CompositeKey3}

import es.tid.cosmos.common.{ConfigComponent, MySqlDatabase, MySqlConnDetails, SqlDatabase}
import es.tid.cosmos.servicemanager.{ClusterName, ServiceDescription, ClusterUser}
import es.tid.cosmos.servicemanager.clusters._

private[servicemanager] trait SqlClusterDaoComponent extends ClusterDaoComponent {
  this: ConfigComponent =>
  private val connDetails = MySqlConnDetails.fromConfig(config)
  private val db = new MySqlDatabase(connDetails)
  private val migrator = new Migrator(
    connDetails.asJdbc,
    connDetails.username,
    connDetails.password,
    new MysqlDatabaseAdapter(schemaNameOpt = None))
  migrator.migrate(
    InstallAllMigrations,
    packageName = "es.tid.cosmos.servicemanager.ambari.migrations",
    searchSubPackages = false)
  val clusterDao: ClusterDao = new SqlClusterDao(db)
}

private[ambari] case class ClusterEntity(
    override val id: String,
    name: String,
    size: Int,
    @Column("name_node") nameNode: Option[String],
    state: String,
    reason: Option[String]
) extends KeyedEntity[String] {

  lazy val users: OneToMany[ClusterUserEntity] = SqlClusterDao.clusterToUsers.left(this)
  lazy val services: OneToMany[ClusterServiceEntity] = SqlClusterDao.clusterToServices.left(this)
}

private[ambari] case class HostEntity(
    @Column("cluster_id") clusterId: String,
    name: String,
    ip: String
)

private[ambari] case class ClusterUserEntity(
    @Column("user_name") userName: String,
    @Column("public_key") publicKey: String,
    @Column("ssh_enabled") sshEnabled: Boolean,
    @Column("hdfs_enabled") hdfsEnabled: Boolean,
    @Column("is_sudoer") isSudoer: Boolean
) extends KeyedEntity[Long] {

  override val id: Long = 0
  @Column("cluster_id") val clusterId: String = ""
  lazy val users: ManyToOne[ClusterEntity] = SqlClusterDao.clusterToUsers.right(this)

  def toClusterUser = ClusterUser(userName, publicKey, sshEnabled, hdfsEnabled, isSudoer)
}

private[ambari] object ClusterUserEntity {

  def apply(user: ClusterUser): ClusterUserEntity = ClusterUserEntity(
    user.username,
    user.publicKey,
    user.sshEnabled,
    user.hdfsEnabled,
    user.isSudoer
  )
}

private[ambari] case class ClusterServiceEntity(name: String) extends KeyedEntity[Long] {
  override val id: Long = 0
  @Column("cluster_id") val clusterId: String = ""
  lazy val services: ManyToOne[ClusterEntity] = SqlClusterDao.clusterToServices.right(this)
}

object HostEntityTypes {
  type MasterEntity = HostEntity with MasterKey
  type SlaveEntity = HostEntity with SlaveKey
  trait MasterKey extends KeyedEntity[String]{
    def clusterId: String
    override def id = clusterId
  }
  trait SlaveKey extends KeyedEntity[CompositeKey3[String, String, String]] {
    def name: String
    def clusterId: String
    def ip: String
    override def id = CompositeKey3(name, clusterId, ip)
  }
}

private[ambari] class SqlClusterDao(db: SqlDatabase) extends ClusterDao {
  import SqlClusterDao._

  override def getDescription(id: ClusterId): Option[MutableClusterDescription] = newTransaction {
    from(clusterState)(c => where(c.id === id.toString) select(c.id))
      .headOption
      .map(_ => new SqlMutableClusterDescription(id, this))
  }

  override def registerCluster(
    id: ClusterId,
    name: ClusterName,
    size: Int,
    services: Set[ServiceDescription]): MutableClusterDescription =
    newTransaction {
      val cluster = clusterState.insert(ClusterEntity(
        id = id.toString,
        name = name.underlying,
        size = size,
        nameNode = None,
        state = Provisioning.name,
        reason = None
      ))
      services.foreach(service => cluster.services.associate(ClusterServiceEntity(service.name)))
      new SqlMutableClusterDescription(id, this)
    }

  override def ids: Seq[ClusterId] = newTransaction {
     from(clusterState)(select(_)).map(c => ClusterId(c.id)).toSeq
  }

  override def getUsers(clusterId: ClusterId): Option[Set[ClusterUser]] = newTransaction {
    from(clusterState)(c => where(c.id === clusterId.id) select(c)).headOption.map(
      cluster => cluster.users.map(_.toClusterUser).toSet
    )
  }

  override def setUsers(clusterId: ClusterId, users: Set[ClusterUser]) = newTransaction {
    from(clusterState)(c => where(c.id === clusterId.id) select(c)).headOption match {
      case Some(cluster) => {
        cluster.users.deleteAll
        users.foreach(usr => cluster.users.associate(ClusterUserEntity(usr)))
      }
      case None => throw new IllegalArgumentException(s"no cluster was found for $clusterId")
    }
  }

  private[ambari] def newTransaction[A](a: =>A) = {
    val session = db.newSession
    try {
      transaction(session)(a)
    } finally {
      session.close
    }
  }
}

private[ambari] object SqlClusterDao extends Schema {
  import scala.language.postfixOps
  import HostEntityTypes._

  private[ambari] val clusterState = table[ClusterEntity]("cluster_state")
  private[ambari] val clusterUsers = table[ClusterUserEntity]("cluster_user")
  private[ambari] val masters = table[MasterEntity]("cluster_master")
  private[ambari] val slaves = table[SlaveEntity]("cluster_slave")
  private[ambari] val services = table[ClusterServiceEntity]("cluster_service")
  private[ambari] val clusterToUsers = oneToManyRelation(clusterState, clusterUsers)
    .via((c, u) => c.id === u.clusterId)
  private[ambari] val clusterToServices = oneToManyRelation(clusterState, services)
      .via((c, s) => c.id === s.clusterId)

  override def applyDefaultForeignKeyPolicy(foreignKeyDeclaration: ForeignKeyDeclaration) =
    foreignKeyDeclaration.constrainReference

  clusterToUsers.foreignKeyDeclaration.constrainReference(onDelete cascade)
  clusterToServices.foreignKeyDeclaration.constrainReference(onDelete cascade)

  on(clusterUsers)(usr => declare(
    usr.id is autoIncremented("cluster_users_id_seq")
  ))
  on(services)(srv => declare(
    srv.id is autoIncremented("cluster_services_id_seq")
  ))
}

