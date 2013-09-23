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
import java.util.Date
import scala.util.Random

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.ClusterAssignment
import es.tid.cosmos.servicemanager._

class ClustersDisplayOrderTest extends FlatSpec with MustMatchers {

  case class SimpleDescription(
      override val name: String,
      override val state: ClusterState,
      override val size: Int = 2 + Random.nextInt(40)
    ) extends ClusterDescription {
    override val id = ClusterId()
    private val failedFuture = Future.failed(new NoSuchElementException())
    override val nameNode_> = failedFuture
    override val master_> = failedFuture
    override val slaves_> = failedFuture
  }

  /**
   * Create a ClusterReference with a given name and state and randomize
   * the other fields.
   */
  def makeRef(name: String, state: ClusterState): ClusterReference = {
    val description = SimpleDescription(name, state)
    val randomOwner = Random.nextLong()
    val randomDate = new Date(Random.nextLong())
    ClusterReference(description, ClusterAssignment(description.id, randomOwner, randomDate))
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
        val cluster1 = makeRef("cluster2", state1)
        val cluster2 = makeRef("cluster1", state2)
        ClustersDisplayOrder.compare(cluster1, cluster2) must be < 0
        ClustersDisplayOrder.compare(cluster2, cluster1) must be > 0
        ClustersDisplayOrder.compare(cluster1, cluster1) must equal (0)
      }
    }
  }

  it must "order then by name" in {
    ClustersDisplayOrder.compare(
      makeRef("cluster 1", Running),
      makeRef("cluster 2", Running)) must be < 0
    ClustersDisplayOrder.compare(
      makeRef("cluster 2", Terminating),
      makeRef("cluster 1", Terminating)) must be > 0
    ClustersDisplayOrder.compare(
      makeRef("cluster a", Terminated),
      makeRef("cluster a", Terminated)) must equal (0)
  }
}
