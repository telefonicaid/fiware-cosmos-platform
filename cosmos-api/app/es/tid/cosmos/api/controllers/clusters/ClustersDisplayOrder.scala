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

package es.tid.cosmos.api.controllers.clusters

import es.tid.cosmos.servicemanager._

/**
 * Sort ClusterReferences by state and then by name
 */
object ClustersDisplayOrder extends Ordering[ClusterDescription] {
  
  def compare(left: ClusterDescription, right: ClusterDescription): Int = {
    val stateComparison = compareStates(left.state, right.state)
    if (stateComparison != 0) stateComparison
    else left.name.compare(right.name)
  }

  private val stateOrder = Map[ClusterState, Int](
    Provisioning -> 0,
    Running -> 1,
    Terminating -> 2,
    Terminated -> 3
  ).withDefaultValue(4)

  private def compareStates(left: ClusterState, right: ClusterState): Int =
    stateOrder(left).compare(stateOrder(right))
}
