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

import scala.concurrent.Future

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import es.tid.cosmos.servicemanager._

class ClustersDisplayOrderTest extends FlatSpec with MustMatchers {

  case class SimpleDescription(
      override val name: String,
      override val state: ClusterState,
      override val size: Int = 10
    ) extends ClusterDescription {
    override val id = ClusterId()
    private val failedFuture = Future.failed(new NoSuchElementException())
    override val nameNode_> = failedFuture
    override val master_> = failedFuture
    override val slaves_> = failedFuture
  }

  object FailedState extends Failed(new IllegalStateException())

  "Clusters display order" must "order first by state" in {
    Seq(
      (Provisioning, Running),
      (Running, Terminating),
      (Terminating, Terminated),
      (Terminated, FailedState),
      (Provisioning, Terminating),
      (Running, FailedState)
    ).foreach {
      case (state1, state2) => {
        val cluster1 = SimpleDescription("cluster2", state1)
        val cluster2 = SimpleDescription("cluster1", state2)
        ClustersDisplayOrder.compare(cluster1, cluster2) must be < 0
        ClustersDisplayOrder.compare(cluster2, cluster1) must be > 0
        ClustersDisplayOrder.compare(cluster1, cluster1) must equal (0)
      }
    }
  }

  it must "order then by name" in {
    ClustersDisplayOrder.compare(
      SimpleDescription("cluster 1", Running),
      SimpleDescription("cluster 2", Running)) must be < 0
    ClustersDisplayOrder.compare(
      SimpleDescription("cluster 2", Terminating),
      SimpleDescription("cluster 1", Terminating)) must be > 0
    ClustersDisplayOrder.compare(
      SimpleDescription("cluster a", Terminated),
      SimpleDescription("cluster a", Terminated)) must equal (0)
  }

  it must "not care about other fields" in {
    val cluster = SimpleDescription("cluster a", Running)
    ClustersDisplayOrder.compare(cluster, cluster.copy(size = 11)) must equal (0)
  }
}
