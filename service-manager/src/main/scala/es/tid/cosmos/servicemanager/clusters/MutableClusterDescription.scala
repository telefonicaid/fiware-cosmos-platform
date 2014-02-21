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

  /** This function handles any errors in the body by marking the cluster as failed. */
  def withFailsafe[A](body: Future[A]): Future[A] = body.transform(identity, err => {
    state = Failed(err)
    err
  })

  final def view: ImmutableClusterDescription = new ImmutableClusterDescription(
    id, name, size, state, nameNode, master, slaves, users, services)
}
