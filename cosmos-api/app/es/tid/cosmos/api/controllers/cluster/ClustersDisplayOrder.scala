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

package es.tid.cosmos.api.controllers.cluster

import es.tid.cosmos.servicemanager.clusters._

/**
 * Sort ClusterReferences by state and then by name
 */
object ClustersDisplayOrder extends Ordering[ClusterReference] {

  def compare(left: ClusterReference, right: ClusterReference): Int = {
    val stateComparison = compareStates(left.description.state, right.description.state)
    if (stateComparison != 0) stateComparison
    else left.description.name.underlying.compare(right.description.name.underlying)
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
