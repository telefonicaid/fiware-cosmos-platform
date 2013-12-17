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

import es.tid.cosmos.servicemanager.clusters._
import java.net.URI
import scala.Some
import es.tid.cosmos.servicemanager.clusters.HostDetails
import es.tid.cosmos.servicemanager.ClusterUser

class InMemoryClusterDescription(
    override val id: ClusterId,
    private var clusterName: String,
    private var clusterSize: Int) extends MutableClusterDescription {
  def name_=(name: String) {
    clusterName = name
  }

  var nameNode_ : Option[URI] = None
  override def nameNode: Option[URI] = nameNode_

  override val size = clusterSize

  var state_ : ClusterState = Provisioning
  def state_=(state: ClusterState) {
    state_ = state
  }

  override def state: ClusterState = state_

  def nameNode_=(nameNode: URI) {
    nameNode_ = Some(nameNode)
  }

  override def name: String = clusterName

  var master_ : Option[HostDetails] = None
  override def master: Option[HostDetails] = master_

  def master_=(master: HostDetails) {
    master_ = Some(master)
  }

  var slaves_ : Seq[HostDetails] = Seq()
  override def slaves: Seq[HostDetails] = slaves_

  def slaves_=(slaves: Seq[HostDetails]) {
    slaves_ = slaves
  }

  var users_ : Option[Set[ClusterUser]] = None
  override def users: Option[Set[ClusterUser]] = users_

  def users_=(users: Set[ClusterUser]) {
    users_ = Some(users)
  }
}
