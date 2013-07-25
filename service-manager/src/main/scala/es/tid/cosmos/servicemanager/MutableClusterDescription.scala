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

import java.net.URI
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.{promise, Future}
import scala.math.max

import es.tid.cosmos.servicemanager.ambari.machines._
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
    val machines_> : Future[Seq[MachineState]],
    val nameNode_> : Future[URI]) {
  private val creation_> = Future.sequence(List(deployment_>, machines_>))
  private val master_> = mapMaster(machines_>, toHostInfo)
  private val slaves_> = mapSlaves(machines_>, toHostInfo)

  def view: ClusterDescription = new ClusterDescription {
    override val state: ClusterState = MutableClusterDescription.this.state
    override val size: Int = MutableClusterDescription.this.size
    override val name: String = MutableClusterDescription.this.name
    override val id: ClusterId = MutableClusterDescription.this.id
    override val nameNode_> : Future[URI] = MutableClusterDescription.this.nameNode_>
    override val master_> : Future[HostDetails] = MutableClusterDescription.this.master_>
    override val slaves_> : Future[Seq[HostDetails]] = MutableClusterDescription.this.slaves_>

    override def equals(other: Any) = other match {
      case that: ClusterDescription =>
        this.state == that.state && this.size == that.size &&
          this.name == that.name && this.id == that.id
      case _ => false
    }
  }

  @volatile private var _state: ClusterState = Provisioning
  @volatile private var nextState_> = promise[ClusterState]

  def state = _state
  def state_=(newState: ClusterState) = synchronized {
    _state = newState
    nextState_>.success(newState)
    nextState_> = promise[ClusterState]
  }

  /**
   * Signals that the cluster is entering termination
   *
   * @param termination_> The termination future
   */
  def terminate(termination_> : Future[Any]) {
    state = Terminating
    for (
      _ <- creation_>;
      _ <- termination_>) {
      state = Terminated
    }

    termination_>.onFailure({case err => { state = Failed(err) }})
  }

  /**
   * Future next state of the cluster
   */
  def nextState: Future[ClusterState] = nextState_>.future

  /**
   * Future to be fulfilled whenever the cluster reaches a given target state.
   *
   * @param targetState     A predicate indicating whether expected state is reached
   * @param maxTransitions  Maximum transitions allowed. Zero means unlimited.
   * @return                Future to wait for targetState
   */
  def whenInState(targetState: ClusterState => Boolean, maxTransitions: Int): Future[Unit] =
    synchronized {
      if (targetState(state)) Future.successful(())
      else nextState.flatMap(state => (maxTransitions, targetState(state)) match {
        case (_, true) => Future.successful(())
        case (1, _) => throw new IllegalStateException(s"Max transitions to reach target state")
        case (0, _) => whenInState(targetState, 0)
        case _ => whenInState(targetState, maxTransitions - 1)
      })
    }

  /**
   * Future to be fulfilled whenever the cluster reaches a given target state.
   *
   * @param targetState     The state expected to be reached
   * @param maxTransitions  Maximum transitions allowed. Zero means unlimited.
   * @return                Future to wait for targetState
   */
  def whenInState(targetState: ClusterState, maxTransitions: Int = 0): Future[Unit] =
    whenInState(state => state == targetState, maxTransitions)

  creation_>.onSuccess({case _ => state = Running})
  creation_>.onFailure({case err => { state = Failed(err) }})
}
