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

package es.tid.cosmos.servicemanager

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future

import es.tid.cosmos.platform.ial.MachineState

/**
 * Provides up-to-date information on the state of a cluster.
 *
 * @constructor
 * @param id            Id of the cluster
 * @param name          Name of the cluster
 * @param size          Size of the cluster
 * @param deployment_>  Future that represents the deployment of the cluster
 * @param machines_>    Future that represents the machines of the cluster.
 *                      Used to terminate the cluster at the IAL level
 */
class MutableClusterDescription(
    val id: ClusterId,
    val name: String,
    val size: Int,
    val deployment_> : Future[Any],
    val machines_> : Future[Seq[MachineState]]) {

  def view: ClusterDescription = new ClusterDescription {
    override val state: ClusterState = MutableClusterDescription.this.state
    override val size: Int = MutableClusterDescription.this.size
    override val name: String = MutableClusterDescription.this.name
    override val id: ClusterId = MutableClusterDescription.this.id
  }

  @volatile var state: ClusterState = Provisioning

  /**
   * Signals that the cluster is entering termination
   *
   * @param termination_> The termination future
   */
  def terminate(termination_> : Future[Any]) {
    state = Terminating
    for (
      _ <- deployment_>;
      _ <- termination_>) {
      state = Terminated
    }

    termination_>.onFailure({case err => { state = new Failed(err)}})
  }

  deployment_>.onSuccess({case _ => state = Running})
  deployment_>.onFailure({case err => { state = new Failed(err) }})
}
