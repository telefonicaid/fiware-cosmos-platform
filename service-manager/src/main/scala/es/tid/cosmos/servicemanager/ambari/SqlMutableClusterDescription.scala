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

import org.squeryl.PrimitiveTypeMode._
import org.squeryl.dsl.ast.UpdateAssignment

import es.tid.cosmos.servicemanager.ambari.HostEntityTypes._
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.ClusterUser

/**
  * This class provides a mutable cluster description which reads directly from the SQL DAO
  * and writes any changes to the DAO too
  * @param id The ID of the cluster
  * @param dao The DAO that backs the description of the cluster
  */
private[ambari] class SqlMutableClusterDescription(
    override val id: ClusterId,
    dao: SqlClusterDao) extends MutableClusterDescription  {

  import SqlClusterDao._

  override def name: String = getField(_.name)

  override def name_=(name: String) {
    setField(_.name := name)
  }

  override def size: Int = getField(_.size)

  override def state: ClusterState = getField(c => (c.state, c.reason)) match {
    case (Provisioning.name, _) => Provisioning
    case (Running.name, _) => Running
    case (Terminating.name, _) => Terminating
    case (Terminated.name, _) => Terminated
    case (Failed.name, Some(reason)) => Failed(reason)
  }

  override def state_=(state: ClusterState) {
    setFields(c => Seq(c.state := state.name, c.reason := (state match {
      case Failed(reason) => Some(reason)
      case _ => None
    })))
  }

  override def nameNode: Option[URI] = getField(_.nameNode).map(new URI(_))

  override def nameNode_=(nameNode: URI) {
    setField(_.nameNode := Some(nameNode.toString))
  }

  override def master: Option[HostDetails] = dao.newTransaction {
    from(masters)(m => where (m.clusterId === this.id.toString) select(m))
      .headOption.map(m => HostDetails(m.name, m.ip))
  }

  override def master_=(master: HostDetails) {
    dao.newTransaction {
      masters.insertOrUpdate(new HostEntity(this.id.toString, master.hostname, master.ipAddress)
        with MasterKey)
    }
  }

  override def slaves: Seq[HostDetails] = dao.newTransaction {
    from(SqlClusterDao.slaves)(s => where (s.clusterId === this.id.toString) select(s))
      .map(s => HostDetails(s.name, s.ip)).toSeq
  }

  override def slaves_=(slaves: Seq[HostDetails]) {
    dao.newTransaction {
      slaves.foreach { h =>
        SqlClusterDao.slaves.insertOrUpdate(new HostEntity(this.id.toString, h.hostname, h.ipAddress)
          with SlaveKey)
      }
    }
  }

  override def users: Option[Set[ClusterUser]] = dao.getUsers(id)

  override def users_=(users: Set[ClusterUser]) = dao.setUsers(id, users)

  private def getField[A](field: ClusterEntity => A) = dao.newTransaction {
    from(clusterState)(c => where (c.id === this.id.toString) select(field(c))).single
  }

  private def setField(setter: ClusterEntity => UpdateAssignment) {
    setFields(c => Seq(setter(c)))
  }

  private def setFields(setter: ClusterEntity => Seq[UpdateAssignment]): Unit = dao.newTransaction {
    update(clusterState)(c => where (c.id === this.id.toString) set(setter(c): _*))
  }
}
