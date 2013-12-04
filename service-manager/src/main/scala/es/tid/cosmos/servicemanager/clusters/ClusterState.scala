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

sealed trait ClusterState {
  val name: String
  val descLine: String
  val canTerminate: Boolean
}

case object Provisioning extends ClusterState {
  val name: String = "provisioning"
  val descLine: String = "Cluster is acquiring and configuring resources"
  val canTerminate: Boolean = false
}

case object Running extends ClusterState {
  val name: String = "running"
  val descLine: String = "Cluster is ready"
  val canTerminate: Boolean = true
}

case object Terminating extends ClusterState {
  val name: String = "terminating"
  val descLine: String = "Releasing cluster resources"
  val canTerminate: Boolean = false
}

case object Terminated extends ClusterState {
  val name: String = "terminated"
  val descLine: String = "The cluster no longer exists"
  val canTerminate: Boolean = false
}

case class Failed(reason: String) extends ClusterState {
  val name: String = Failed.name
  val descLine: String = s"A cluster operation has failed: $reason"
  val canTerminate: Boolean = true
}

object Failed {
  def apply(reason: Throwable): Failed = Failed(reason.getMessage)
  val name: String = "failed"
}

object ClusterState {
  /** The cluster states indicating that the cluster is alive and active.
    * Any states not in this set indicate that the cluster is not available due to
    * e.g termination, failure, etc
    */
  val ActiveStates = Set[ClusterState](Provisioning, Running, Terminating)
}
