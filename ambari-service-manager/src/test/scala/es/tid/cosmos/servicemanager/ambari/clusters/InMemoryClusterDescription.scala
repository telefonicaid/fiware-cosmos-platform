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

package es.tid.cosmos.servicemanager.ambari.clusters

import java.net.URI

import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}
import es.tid.cosmos.servicemanager.clusters._

class InMemoryClusterDescription(
    override val id: ClusterId,
    override var name: ClusterName,
    private var clusterSize: Int,
    override var services : Set[String]) extends MutableClusterDescription {

  var nameNode_ : Option[URI] = None
  override def nameNode: Option[URI] = nameNode_

  def nameNode_=(nameNode: URI) {
    nameNode_ = Some(nameNode)
  }

  override val size = clusterSize

  override var state : ClusterState = Provisioning

  var master_ : Option[HostDetails] = None
  override def master: Option[HostDetails] = master_

  def master_=(master: HostDetails) {
    master_ = Some(master)
  }

  override var slaves : Seq[HostDetails] = Seq.empty

  var users_ : Option[Set[ClusterUser]] = None
  override def users: Option[Set[ClusterUser]] = users_

  def users_=(users: Set[ClusterUser]) {
    users_ = Some(users)
  }

  override var blockedPorts: Set[Int] = Set.empty
}
