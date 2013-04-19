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

sealed trait ClusterState {
  val name: String
  val descLine: String
}

case object Provisioning extends ClusterState {
  val name: String = "provisioning"
  val descLine: String = "Cluster is acquiring and configuring resources"
}

case object Running extends ClusterState {
  val name: String = "running"
  val descLine: String = "Cluster is ready"
}

case object Terminating extends ClusterState {
  val name: String = "terminating"
  val descLine: String = "Releasing cluster resources"
}

case object Terminated extends ClusterState {
  val name: String = "terminated"
  val descLine: String = "The cluster no longer exists"
}

case class Failed(reason: Throwable) extends ClusterState {
  val name: String = "failed"
  val descLine: String = s"A cluster operation has failed: ${reason.getMessage}"
}
