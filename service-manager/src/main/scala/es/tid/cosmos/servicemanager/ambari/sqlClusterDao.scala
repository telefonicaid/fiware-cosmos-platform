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
import org.squeryl.{KeyedEntity, Schema}
import org.squeryl.PrimitiveTypeMode._
import org.squeryl.annotations.Column
import org.squeryl.dsl.CompositeKey3

import es.tid.cosmos.platform.common.{MySqlConnDetails, MySqlDatabase, ConfigComponent, SqlDatabase}
import es.tid.cosmos.servicemanager.clusters._

private[servicemanager] trait SqlClusterDaoComponent extends ClusterDaoComponent {
  this: ConfigComponent =>
  private val connDetails = MySqlConnDetails.fromConfig(config)
  private val migrator = new Migrator(
    connDetails.asJdbc,
    connDetails.username,
    connDetails.password,
    new MysqlDatabaseAdapter(schemaNameOpt = None))
  migrator.migrate(
    InstallAllMigrations,
    packageName = "es.tid.cosmos.servicemanager.ambari.migrations",
    searchSubPackages = false)
  private val db = new MySqlDatabase(connDetails)
  val clusterDao: ClusterDao = new SqlClusterDao(db)
}

private[ambari] case class ClusterEntity(
    id: String,
    name: String,
    size: Int,
    @Column("name_node")
    nameNode: Option[String],
    state: String,
    reason: Option[String])

private[ambari] case class HostEntity(@Column("cluster_id") clusterId: String, name: String, ip: String)

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

private[ambari] class SqlClusterDao(db: SqlDatabase) extends ClusterDao with Schema {
  import HostEntityTypes._

  override def getDescription(id: ClusterId): Option[MutableClusterDescription] = newTransaction {
    from(cluster_state)(c => where(c.id === id.toString) select(c.id))
      .headOption
      .map(_ => new SqlMutableClusterDescription(id, this))
  }

  override def registerCluster(id: ClusterId, name: String, size: Int): MutableClusterDescription =
    newTransaction {
      cluster_state.insert(ClusterEntity(id = id.toString, name = name, size = size,
        nameNode = None, state = Provisioning.name, reason = None))
      new SqlMutableClusterDescription(id, this)
    }

  override def ids: Seq[ClusterId] = newTransaction {
     from(cluster_state)(select(_)).map(c => ClusterId(c.id)).toSeq
  }

  private[ambari] val cluster_state = table[ClusterEntity]("cluster_state")
  private[ambari] val masters = table[MasterEntity]("master")
  private[ambari] val slaves = table[SlaveEntity]("slave")

  private[ambari] def newTransaction[A](a: =>A) = {
    val session = db.newSession
    try {
      transaction(session)(a)
    } finally {
      session.close
    }
  }
}

