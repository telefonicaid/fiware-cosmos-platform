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

import es.tid.cosmos.platform.ial.PreconditionsNotMetException
import es.tid.cosmos.platform.ial.MachineProfile.G1Compute
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.services.{Hdfs, MapReduce}
import es.tid.cosmos.servicemanager.clusters._

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
    def view = new ImmutableClusterDescription(id, name, size, state, nameNode, master, slaves)
  }

  private class TransitioningCluster (
      override val name: String,
      override val size: Int,
      override val id: ClusterId = new ClusterId) extends FakeCluster {

    var currentState: ClusterState = Provisioning
    override def state = currentState
    var nameNode_ : Option[URI] = None
    override def nameNode = nameNode_
    var master_ : Option[HostDetails] = None
    override def master = master_
    var slaves_ : Seq[HostDetails] = Seq()
    override def slaves = slaves_

    def completeProvision() {
      if (currentState == Provisioning){
        currentState = Running
        nameNode_ = Some(new URI(s"hdfs://10.0.0.${Random.nextInt(256)}:8084"))
        master_ = Some(HostDetails("fakeHostname", "fakeAddress"))
        slaves_ = (1 to (size-1)).map(i => HostDetails(s"fakeHostname$i", s"fakeAddress$i"))
      }
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
    override val nameNode: Option[URI] = None
    override val master: Option[HostDetails] = None
    override val slaves: Seq[HostDetails] = Seq()
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
        name = "cluster0", size = 10)
      put(cluster0.id, cluster0)
      val clusterInProgress = new InProgressCluster(
        id = MockedServiceManager.InProgressClusterId,
        name = "clusterInProgress",
        size = 10
      )
      put(clusterInProgress.id, clusterInProgress)
    }

  override def clusterIds: Seq[ClusterId] = clusters.keySet.toSeq

  override def createCluster(
      name: String,
      clusterSize: Int,
      serviceDescriptions: Seq[ServiceDescriptionType],
      users: Seq[ClusterUser],
      preConditions: ClusterExecutableValidation): ClusterId = {
    val clusterId = ClusterId()
    preConditions(clusterId)().fold(
      fail = errors => throw PreconditionsNotMetException(G1Compute, clusterSize, errors.list),
      succ = _ => ()
    )
    val cluster = new TransitioningCluster(name, clusterSize, clusterId)
    clusters.put(cluster.id, cluster)
    cluster.id
  }

  override def describeCluster(clusterId: ClusterId): Option[ImmutableClusterDescription] =
    if (clusterId == persistentHdfsId && persistentHdfsCluster.enabled) Some(persistentHdfsCluster)
    else clusters.get(clusterId).map(_.view)

  override def terminateCluster(id: ClusterId): Future[Unit] = {
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

  private val persistentHdfsCluster = new ImmutableClusterDescription(
    id = ClusterId("PersistendHdfsId"),
    nameNode = Some(MockedServiceManager.PersistentHdfsUrl),
    size = 4,
    state = Running,
    name = "Persistent storage cluster",
    master = Some(HostDetails("stoarge", "storageAddress")),
    slaves = (1 to 3).map(i => HostDetails(s"storage$i", s"storageAddress$i"))
  ) {
    @volatile var enabled: Boolean = false
  }

  override def persistentHdfsId: ClusterId = persistentHdfsCluster.id

  override def setUsers(clusterId: ClusterId, users: Seq[ClusterUser]): Future[Unit] = successful()

  override def deployPersistentHdfsCluster(): Future[Unit] = {
    persistentHdfsCluster.enabled = true
    successful()
  }

  override def describePersistentHdfsCluster(): Option[ImmutableClusterDescription] =
    Some(persistentHdfsCluster)

  override def terminatePersistentHdfsCluster(): Future[Unit] = successful()

  override def clusterNodePoolCount: Int = 10
}

object MockedServiceManager {
  val DefaultClusterId = new ClusterId("00000000000000000000000000000000")
  val InProgressClusterId = new ClusterId("11111111111111111111111111111111")
  val PersistentHdfsUrl = new URI("hdfs://10.0.0.6:8084")
}
