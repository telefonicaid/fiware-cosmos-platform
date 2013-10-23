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

package es.tid.cosmos.api.mocks.servicemanager

import java.net.URI
import scala.collection.mutable
import scala.concurrent._
import scala.concurrent.duration._
import scala.concurrent.Future.successful
import scala.concurrent.ExecutionContext.Implicits.global
import scala.language.{postfixOps, reflectiveCalls}
import scala.util.Random

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.services.{Hdfs, MapReduce}

/**
 * In-memory, simulated service manager.
 *
 * @param transitionDelay Cluster state transition delay in millis
 */
class MockedServiceManager(transitionDelay: Int) extends ServiceManager {

  trait FakeCluster extends ClusterDescription {
    var currentState: ClusterState
    def completeProvision()
    def completeTermination()
  }

  private class TransitioningCluster (
      override val name: String,
      override val size: Int,
      override val id: ClusterId = new ClusterId) extends FakeCluster {

    var currentState: ClusterState = Provisioning
    override def state = currentState
    override def nameNode_> = successful(new URI(s"hdfs://10.0.0.${Random.nextInt(256)}:8084"))
    override val master_> = successful(HostDetails("fakeHostname", "fakeAddress"))
    override val slaves_> = successful(Seq(HostDetails("fakeHostname", "fakeAddress")))

    def completeProvision() {
      if (currentState == Provisioning) currentState = Running
    }

    def completeTermination() {
      if (currentState == Terminating) currentState = Terminated
    }

    defer(transitionDelay, completeProvision())
  }

  private class InProgressCluster(
       override val name: String,
       override val size: Int,
       override val id: ClusterId = new ClusterId) extends FakeCluster {

    private val resolutionDelay = 10 seconds
    override var currentState: ClusterState = Provisioning
    override def state = currentState
    override def nameNode_> = successful(new URI(s"hdfs://10.0.0.${Random.nextInt(256)}:8084"))
    override def master_> = defer(HostDetails("fakeHostname", "fakeAddress"))
    override def slaves_> = defer(Seq(HostDetails("fakeHostname", "fakeAddress")))
    override def completeProvision() {}
    override def completeTermination() {}

    def defer[T](result: T):Future[T] =
          future { blocking { Thread.sleep(resolutionDelay.toMillis); result } }
  }

  override type ServiceDescriptionType = ServiceDescription

  override val services: Seq[ServiceDescriptionType] = Seq(Hdfs, MapReduce)

  private val clusters: mutable.Map[ClusterId, FakeCluster] =
    new mutable.HashMap[ClusterId, FakeCluster]
      with mutable.SynchronizedMap[ClusterId, FakeCluster] {
      val cluster0 = new TransitioningCluster(
        id = MockedServiceManager.DefaultClusterId,
        name = "cluster0", size = 100)
      put(cluster0.id, cluster0)
      val clusterInProgress = new InProgressCluster(
        id = MockedServiceManager.InProgressClusterId,
        name = "clusterInProgress",
        size = 100
      )
      put(clusterInProgress.id, clusterInProgress)
    }

  def clusterIds: Seq[ClusterId] = clusters.keySet.toSeq

  override def createCluster(
      name: String,
      clusterSize: Int,
      serviceDescriptions: Seq[ServiceDescriptionType],
      users: Seq[ClusterUser]): ClusterId = {
    val cluster = new TransitioningCluster(name, clusterSize)
    clusters.put(cluster.id, cluster)
    cluster.id
  }

  def describeCluster(clusterId: ClusterId): Option[ClusterDescription] =
    if (clusterId == persistentHdfsId && persistentHdfsCluster.enabled) Some(persistentHdfsCluster)
    else clusters.get(clusterId)

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

  private val persistentHdfsCluster = new ClusterDescription {
    @volatile var enabled: Boolean = false
    override val id = ClusterId("PersistendHdfsId")
    override val nameNode_> = Future.successful(MockedServiceManager.PersistentHdfsUrl)
    override val state = Running
    override val size = 4
    override val name = "Persistent storage cluster"
    override val master_> = successful(HostDetails("stoarge", "storageAddress"))
    override val slaves_> = successful(Seq(HostDetails("storage", "storageAddress")))
  }

  def persistentHdfsId: ClusterId = persistentHdfsCluster.id

  def setUsers(clusterId: ClusterId, users: Seq[ClusterUser]): Future[Unit] = successful()

  def deployPersistentHdfsCluster(): Future[Unit] = {
    persistentHdfsCluster.enabled = true
    successful()
  }

  def describePersistentHdfsCluster(): Option[ClusterDescription] = Some(persistentHdfsCluster)

  def terminatePersistentHdfsCluster(): Future[Unit] = successful()

  def addUsers(clusterId: ClusterId, users: ClusterUser*): Future[Unit] = successful()

  def refresh() = successful()
}

object MockedServiceManager {
  val DefaultClusterId = new ClusterId("00000000000000000000000000000000")
  val InProgressClusterId = new ClusterId("11111111111111111111111111111111")
  val PersistentHdfsUrl = new URI("hdfs://10.0.0.6:8084")
}
