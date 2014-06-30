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

package es.tid.cosmos.servicemanager.clusters

import java.net.URI
import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}

/** A mutable description of a cluster that includes setters */
trait MutableClusterDescription extends ClusterDescription {
  override val id: ClusterId
  override def name: ClusterName
  def name_=(name: ClusterName): Unit
  override def state: ClusterState
  def state_=(state: ClusterState): Unit
  override def nameNode: Option[URI]
  def nameNode_=(nameNode: URI): Unit
  override def master: Option[HostDetails]
  def master_=(master: HostDetails): Unit
  override def slaves: Seq[HostDetails]
  def slaves_=(slaves: Seq[HostDetails]): Unit
  override def users: Option[Set[ClusterUser]]
  def users_=(users: Set[ClusterUser])
  override def services: Set[String]
  def services_=(services: Set[String])
  override def blockedPorts: Set[Int]
  def blockedPorts_=(blockedPorts: Set[Int])

  /** This function handles any errors in the body by marking the cluster as failed. */
  def withFailsafe[A](body: Future[A]): Future[A] = body.transform(identity, err => {
    state = Failed(err)
    err
  })

  final def view: ImmutableClusterDescription = new ImmutableClusterDescription(
    id, name, size, state, nameNode, master, slaves, users, services, blockedPorts)
}
