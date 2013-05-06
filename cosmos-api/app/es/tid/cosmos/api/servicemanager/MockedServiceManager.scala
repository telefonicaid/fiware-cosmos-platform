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

package es.tid.cosmos.api.servicemanager

import java.util.UUID
import scala.concurrent._
import scala.concurrent.ExecutionContext.Implicits.global
import scala.collection.mutable

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.ServiceException
import es.tid.cosmos.servicemanager.services.ServiceDescription

/**
 * In-memory, simulated service manager.
 *
 * @param transitionDelay Cluster state transition delay in millis
 * @author sortega
 */
class MockedServiceManager(transitionDelay: Int) extends ServiceManager {

  private class FakeCluster(override val name: String, override val size: Int,
                            override val id: ClusterId = new ClusterId)
    extends ClusterDescription {

    override def state = currentState
    var currentState: ClusterState = Provisioning

    def completeProvision() {
      if (currentState == Provisioning) currentState = Running
    }

    def completeTermination() {
      if (currentState == Terminating) currentState = Terminated
    }

    defer(transitionDelay, completeProvision())
  }

  private val clusters: mutable.Map[ClusterId, FakeCluster] =
    new mutable.HashMap[ClusterId, FakeCluster]
      with mutable.SynchronizedMap[ClusterId, FakeCluster] {
      val cluster0 = new FakeCluster(
        id = MockedServiceManager.defaultClusterId,
        name = "cluster0", size = 100)
      put(cluster0.id, cluster0)
    }

  def clusterIds: Seq[ClusterId] = clusters.keySet.toSeq

  def createCluster(
    name: String, clusterSize: Int, serviceDescriptions: Seq[ServiceDescription]): ClusterId = {
    val cluster = new FakeCluster(name, clusterSize)
    clusters.put(cluster.id, cluster)
    cluster.id
  }

  def describeCluster(clusterId: ClusterId): Option[ClusterDescription] = clusters.get(clusterId)

  def terminateCluster(id: ClusterId): Future[Unit] = {
    if (!clusters.contains(id))
      throw new ServiceException("Unknown cluster")
    val cluster = clusters.get(id).get
    cluster.currentState = Terminating
    defer(transitionDelay, cluster.completeTermination())
  }

  private def defer(delay: Int, action: => Unit) = {
    future {
      Thread.sleep(delay)
      action
    }
  }
}

object MockedServiceManager {
  val defaultClusterId = new ClusterId(UUID.fromString("00000000-0000-0000-0000-000000000000"))
}
