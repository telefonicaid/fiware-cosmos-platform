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
import scala.concurrent._
import scala.concurrent.duration._
import scala.concurrent.ExecutionContext.Implicits.global
import scala.language.{postfixOps, reflectiveCalls}
import scala.util.Random

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.services.{Hdfs, MapReduce}
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.clusters.ImmutableClusterDescription

/**
 * In-memory, simulated service manager.
 *
 * @param transitionDelay Cluster state transition delay in millis
 */
class MockedServiceManager(
    transitionDelay: FiniteDuration, maxPoolSize: Int = 20) extends ServiceManager {

  import MockedServiceManager._

  class FakeCluster(
      id: ClusterId,
      name: String,
      size: Int,
      initialUsers: Set[ClusterUser],
      services: Set[String],
      initialState: Option[ClusterState] = None) {

    private var state: ClusterState = Provisioning
    private var nameNode: Option[URI] = None
    private var master: Option[HostDetails] = None
    private var slaves: Seq[HostDetails] = Seq.empty
    private var users: Option[Set[ClusterUser]] = None

    def isConsumingMachines: Boolean = state match {
      case Terminated | Failed(_) => false
      case _ => true
    }

    def setUsers(newUsers: Set[ClusterUser]): Future[Unit] = synchronized {
      users = Some(newUsers)
      Future.successful()
    }

    def setState(newState: ClusterState) = synchronized {
      (state, newState) match {
        case (Provisioning, Running) =>
          master = Some(randomHost)
          slaves = randomHostSeq(size)
          nameNode = Some(new URI(s"hdfs://${master.get.ipAddress}:8084"))
          users = Some(initialUsers)
        case _ =>
      }
      state = newState
    }

    def successfulProvision = clusterNodePoolCount >= size

    def view = synchronized {
      ImmutableClusterDescription(id, name, size, state, nameNode, master, slaves, users, services)
    }

    def transitionTo(newState: ClusterState) = future {
      Thread.sleep(transitionDelay.toMillis)
      setState(newState)
    }

    def terminate() = synchronized {
      state = Terminating
      transitionTo(Terminated)
    }

    private def randomHost: HostDetails = {
      val n = Random.nextInt(256) + 1
      HostDetails(s"compute-$n", s"192.168.1.$n")
    }

    private def randomHostSeq(len: Int): Seq[HostDetails] = Seq.fill(len - 1)(randomHost)

    val provisioningFuture =
      if (initialState.isEmpty) {
        if (successfulProvision) transitionTo(Running) else transitionTo(Failed(new Error))
      } else {
        setState(initialState.get)
        Future.successful()
      }
  }

  private var clusters: Map[ClusterId, FakeCluster] = Map.empty
  
  override type ServiceDescriptionType = ServiceDescription

  override def clusterIds: Seq[ClusterId] = clusters.keySet.toSeq

  override val optionalServices: Seq[ServiceDescriptionType] = Seq(Hdfs, MapReduce)

  override def createCluster(
      name: String,
      size: Int,
      serviceDescriptions: Seq[ServiceDescriptionType],
      users: Seq[ClusterUser],
      preConditions: ClusterExecutableValidation): ClusterId = synchronized {
    val id = ClusterId()
    require(preConditions(id).apply().isSuccess, "preconditions were not met")
    defineCluster(ClusterProperties(id, name, size, users.toSet))
    id
  }

  override def describeCluster(id: ClusterId): Option[ImmutableClusterDescription] = synchronized {
    clusters.get(id).map(_.view)
  }

  override def terminateCluster(id: ClusterId): Future[Unit] = synchronized {
    clusters(id).terminate()
  }

  override val persistentHdfsId: ClusterId = PersistentHdfsProps.id

  override def deployPersistentHdfsCluster(): Future[Unit] =
    defineCluster(PersistentHdfsProps).provisioningFuture

  override def listUsers(clusterId: ClusterId): Option[Seq[ClusterUser]] = for {
    cluster <- clusters.get(clusterId)
    users <- cluster.view.users
  } yield users.toSeq

  override def setUsers(clusterId: ClusterId, users: Seq[ClusterUser]): Future[Unit] =
    clusters(clusterId).setUsers(users.toSet)

  override def clusterNodePoolCount: Int = synchronized {
    maxPoolSize - provisionedMachines
  }

  private def provisionedMachines = synchronized {
    clusters.values.filter(_.isConsumingMachines).map(_.view.size).sum
  }

  private def defineCluster(props: ClusterProperties): FakeCluster = synchronized {
    val cluster = new FakeCluster(
      props.id, props.name, props.size, props.users.toSet,  props.services.toSet, props.initialState)
    clusters += props.id -> cluster
    cluster
  }

  defineCluster(DefaultClusterProps)
  defineCluster(InProgressClusterProps)
}

object MockedServiceManager {

  case class ClusterProperties(
    id: ClusterId,
    name: String,
    size: Int,
    users: Set[ClusterUser],
    initialState: Option[ClusterState] = None,
    services: Seq[String] = Seq("HDFS", "MAPREDUCE")
  )

  val PersistentHdfsProps = ClusterProperties(
    id = new ClusterId("persistentHdfs"),
    name = "Persistent HDFS",
    size = 4,
    users = Set(
      ClusterUser.enabled("jsmith", "jsmith-public-key"),
      ClusterUser.enabled("pocahontas", "pocahontas-public-key")
    ),
    initialState = None
  )

  val DefaultClusterProps = ClusterProperties(
    id = new ClusterId(),
    name = "Default cluster",
    size = 4,
    users = Set(ClusterUser.enabled("jsmith", "jsmith-public-key")),
    initialState = Some(Running)
  )

  val InProgressClusterProps = ClusterProperties(
    id = new ClusterId(),
    name = "In progress cluster",
    size = 4,
    users = Set(ClusterUser.enabled("pocahontas", "pocahontas-public-key")),
    initialState = None
  )

  val UnknownClusterProps = ClusterProperties(
    id = new ClusterId(),
    name = "A cluster that doesn't exist",
    size = 4,
    users = Set(ClusterUser.enabled("pocahontas", "pocahontas-public-key")),
    initialState = None
  )
}
