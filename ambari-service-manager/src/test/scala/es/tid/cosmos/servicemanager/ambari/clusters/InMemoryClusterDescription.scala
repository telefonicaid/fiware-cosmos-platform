/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
