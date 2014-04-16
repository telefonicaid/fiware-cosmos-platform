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

import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}

/** A description of a cluster */
trait ClusterDescription {

  /** Unique cluster identifier. */
  def id: ClusterId

  /** Human-readable name of the cluster. */
  def name: ClusterName

  /** Number of machines. */
  def size: Int

  /** The size the cluster should have based on its state.
    * This is a heuristic approach to approximate what the actual size of the cluster should be
    * judging from its state.
    *
    * @return the expected size
    */
  final def expectedSize: Int = state match {
    case Provisioning | Running | Terminating => size
    case Failed(_) => machines.size
    case Terminated => 0
  }

  /** The cluster's machines */
  final def machines: Seq[HostDetails] = (master ++ slaves).toSet.toSeq

  /** Lifecycle state of the cluster. */
  def state: ClusterState

  /** Name node URI with hdfs scheme.
    * For instance: hdfs://localhost:54310
    */
  def nameNode: Option[URI]

  /** The cluster's master node information.
    * @return the master node information
    */
  def master: Option[HostDetails]

  /** The cluster's slave nodes information.
    * @return the slave nodes information
    */
  def slaves: Seq[HostDetails]

  /** The users of this cluster. */
  def users: Option[Set[ClusterUser]]

  /** The services enabled on this cluster */
  def services: Set[String]

  /** Both master and slave host details. */
  def hosts: Set[HostDetails] = (slaves ++ master).toSet

  /** The ports that have been blocked on this cluster */
  def blockedPorts: Set[Int]
}
