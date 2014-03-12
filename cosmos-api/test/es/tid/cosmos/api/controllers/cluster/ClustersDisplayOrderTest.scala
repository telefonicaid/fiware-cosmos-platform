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

import java.sql.Timestamp
import scala.util.Random

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.Cluster
import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}
import es.tid.cosmos.servicemanager.clusters._

class ClustersDisplayOrderTest extends FlatSpec with MustMatchers {

  case class SimpleDescription(
      override val name: ClusterName,
      override val state: ClusterState
    ) extends ClusterDescription {
    override val id = ClusterId.random()
    override val nameNode = None
    override val size = Random.nextInt(40)
    override val master = Some(HostDetails("foo", "bar"))
    override val slaves = HostDetails("foo2", "bar2") +: (1 to (size - 1)).map(
      i => HostDetails(s"host$i", s"ip$i"))
    override val users = Some(Set(ClusterUser("jsmith", "jsmith-public-key")))
    override val services = Set("ServiceA", "ServiceB")
  }

  /** Create a ClusterReference with a given name and state and randomize the other fields. */
  def makeRef(name: String, state: ClusterState): ClusterReference = {
    val description = SimpleDescription(ClusterName(name), state)
    val randomOwner = Random.nextLong()
    val randomDate = new Timestamp(Random.nextLong())
    ClusterReference(description, Cluster(description.id, randomOwner, randomDate))
  }

  object FailedState extends Failed(new IllegalStateException().getMessage)

  "Clusters display order" must "order first by state" in {
    Seq(
      (Provisioning, Running),
      (Running, Terminating),
      (Terminating, Terminated),
      (Terminated, FailedState),
      (Provisioning, Terminating),
      (Running, FailedState)
    ).foreach {
      case (state1, state2) =>
        val cluster1 = makeRef("cluster2", state1)
        val cluster2 = makeRef("cluster1", state2)
        ClustersDisplayOrder.compare(cluster1, cluster2) must be < 0
        ClustersDisplayOrder.compare(cluster2, cluster1) must be > 0
        ClustersDisplayOrder.compare(cluster1, cluster1) must equal (0)
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
