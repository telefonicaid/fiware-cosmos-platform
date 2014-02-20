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
import scala.language.{postfixOps, reflectiveCalls}
import scala.util.Random

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.services.{Hdfs, MapReduce2}
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.clusters.ImmutableClusterDescription

/** In-memory, simulated service manager. */
class MockedServiceManager(maxPoolSize: Int = 20) extends ServiceManager {

  import MockedServiceManager._

  class FakeCluster(
      id: ClusterId,
      name: ClusterName,
      size: Int,
      initialUsers: Set[ClusterUser],
      services: Set[String],
      initialState: Option[ClusterState] = None) {

    private case class Observer(predicate: FakeCluster => Boolean, action: FakeCluster => Unit)

    @volatile private var state: ClusterState =
      if (successfulProvision) initialState.getOrElse(Provisioning) else Failed(new Error)
    @volatile private var nameNode: Option[URI] = None
    @volatile private var master: Option[HostDetails] = None
    @volatile private var slaves: Seq[HostDetails] = Seq.empty
    @volatile private var users: Option[Set[ClusterUser]] = None
    @volatile private var statePromises: Map[ClusterState, Promise[Unit]] = Map.empty
    @volatile private var pendingSetUserOperations: Seq[(Set[ClusterUser], Promise[Unit])] = Seq()
    @volatile private var observers: Set[Observer] = Set.empty

    def isConsumingMachines: Boolean = state match {
      case Terminated => false
      case _ => true
    }

    def setUsers(newUsers: Set[ClusterUser]): Future[Unit] = synchronized {
      val setUserOperation = (newUsers, Promise[Unit]())
      pendingSetUserOperations +:= setUserOperation
      if (autoCompleteSetUserOperations) completeSetUsers()
      setUserOperation._2.future
    }

    def completeSetUsers(): Boolean = { synchronized {
      if (pendingSetUserOperations.isEmpty)
        return false
      for {
        (newUsers, promise) <- pendingSetUserOperations
      } {
        users = Some(newUsers)
        promise.success()
      }
      pendingSetUserOperations = Seq()
      true
    }}

    def setState(newState: ClusterState): Future[Unit] = synchronized {
      statePromises.get(state).foreach(_.success(()))
      (state, newState) match {
        case (_, Running) =>
          master = Some(randomHost)
          slaves = randomHostSeq(size)
          nameNode = Some(new URI(s"hdfs://${master.get.ipAddress}:8084"))
          users = Some(initialUsers)
        case _ =>
      }
      state = newState
      val promise = Promise[Unit]()
      statePromises += state -> promise
      notifyObservers()
      promise.future
    }

    def when(predicate: FakeCluster => Boolean)(action: FakeCluster => Unit) { synchronized {
      observers += Observer(predicate, action)
    }}

    def immediateTransition(from: ClusterState, to: ClusterState) {
      when(_.state == from) {
        _.setState(to)
      }
    }

    def immediateProvision() = immediateTransition(Provisioning, Running)

    def immediateTermination() = immediateTransition(Terminating, Terminated)

    def successfulProvision = clusterNodePoolCount >= size

    def view = synchronized {
      ImmutableClusterDescription(id, name, size, state, nameNode, master, slaves, users, services)
    }

    def transitionFuture: Future[Unit] = synchronized { statePromises(state).future }

    def completeProvisioning() = {
      require(state == Provisioning)
      setState(Running)
    }

    def startTermination() = setState(Terminating)

    def completeTermination() = {
      require(state == Terminating)
      setState(Terminated)
    }

    def makeFail(reason: String) = synchronized {
      val failTransition_> = transitionFuture
      setState(Failed(reason))
      failTransition_>
    }

    private def notifyObservers() {
      for (Observer(pred, act) <- observers if pred(this)) {
        act(this)
      }
    }

    private def randomHost: HostDetails = {
      val n = Random.nextInt(256) + 1
      HostDetails(s"compute-$n", s"192.168.1.$n")
    }

    private def randomHostSeq(len: Int): Seq[HostDetails] = Seq.fill(len - 1)(randomHost)

    setState(state)
  }

  @volatile private var clusters: Map[ClusterId, FakeCluster] = Map.empty
  @volatile var autoCompleteSetUserOperations = true

  override def clusterIds: Seq[ClusterId] = clusters.keySet.toSeq

  override val optionalServices: Seq[ServiceDescription] = Seq(Hdfs, MapReduce2)

  override def createCluster(
      name: ClusterName,
      size: Int,
      serviceDescriptions: Seq[ServiceDescription],
      users: Seq[ClusterUser],
      preConditions: ClusterExecutableValidation): ClusterId = synchronized {
    val id = ClusterId()
    val properties = ClusterProperties(id, name, size, users.toSet)

    val propertiesAfterPreconditions = preConditions(id).apply().fold(
     fail = errors => properties.copy(initialState = Some(Failed(errors.list.mkString(", ")))),
     succ = (_) => properties
    )
    defineCluster(propertiesAfterPreconditions)
    id
  }

  override def describeCluster(id: ClusterId): Option[ImmutableClusterDescription] = synchronized {
    clusters.get(id).map(_.view)
  }

  override def terminateCluster(id: ClusterId): Future[Unit] = synchronized {
    clusters(id).startTermination()
  }

  def makeClusterFail(id: ClusterId, reason: String): Future[Unit] = synchronized {
    clusters(id).makeFail(reason)
  }

  override val persistentHdfsId: ClusterId = PersistentHdfsProps.id

  override def deployPersistentHdfsCluster(): Future[Unit] =
    defineCluster(PersistentHdfsProps).transitionFuture

  override def listUsers(clusterId: ClusterId): Option[Seq[ClusterUser]] = for {
    cluster <- clusters.get(clusterId)
    users <- cluster.view.users
  } yield users.toSeq

  override def setUsers(clusterId: ClusterId, users: Seq[ClusterUser]): Future[Unit] =
    clusters(clusterId).setUsers(users.toSet)

  def completeAllSetUserOperations() = {
    clusterIds.map(clusterId => {
      val cluster = clusters(clusterId)
      cluster.completeSetUsers()
    }).exists(_ == true)
  }

  override def clusterNodePoolCount: Int = maxPoolSize

  def withCluster(clusterId: ClusterId)(action: FakeCluster => Unit) {
    action(clusters(clusterId))
  }

  def defineCluster(props: ClusterProperties): FakeCluster = synchronized {
    val cluster = new FakeCluster(
      props.id, props.name, props.size, props.users.toSet,  props.services.toSet, props.initialState)
    clusters += props.id -> cluster
    cluster
  }
}

object MockedServiceManager {

  case class ClusterProperties(
    id: ClusterId,
    name: ClusterName,
    size: Int,
    users: Set[ClusterUser],
    initialState: Option[ClusterState] = None,
    services: Seq[String] = Seq("HDFS", "MAPREDUCE")
  )

  val PersistentHdfsProps = ClusterProperties(
    id = new ClusterId("persistentHdfs"),
    name = ClusterName("Persistent HDFS"),
    size = 4,
    users = Set(
      ClusterUser.enabled("jsmith", "jsmith-public-key"),
      ClusterUser.enabled("pocahontas", "pocahontas-public-key")
    ),
    initialState = Some(Running)
  )
}
