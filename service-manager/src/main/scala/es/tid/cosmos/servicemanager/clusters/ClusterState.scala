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

sealed trait ClusterState {
  val name: String
  val descLine: String
  val canTerminate: Boolean
  def isActive: Boolean = ClusterState.ActiveStates.contains(this)
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
