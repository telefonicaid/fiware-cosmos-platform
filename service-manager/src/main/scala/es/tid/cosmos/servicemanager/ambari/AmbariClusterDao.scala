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

package es.tid.cosmos.servicemanager.ambari

import scala.concurrent.{Await, Future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._

import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescription
import es.tid.cosmos.servicemanager.ambari.rest.AmbariServer
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.ClusterUser

/**
  * This class wraps a ClusterDao and makes sure that the data in Ambari and the data in the
  * DAO are consistent. If there is an inconsistency the DAO is modified (so we use the Ambari
  * server as the ground truth).
  *
  * @constructor
  * @param dao The DAO that will be wrapped
  * @param ambariServer The Ambari server that provides the ground truth
  * @param allServices A list of all the services the Service Manager supports
  * @param initializationPeriod The maximum duration of the integrity validation
  */
private[ambari] class AmbariClusterDao(
    dao: ClusterDao,
    ambariServer: AmbariServer,
    allServices: Seq[AmbariServiceDescription],
    initializationPeriod: FiniteDuration = 5 minutes)
  extends ClusterDao with ClusterStateResolver {

  override def ids: Seq[ClusterId] = withIntegrity {
    this.dao.ids
  }

  override def getDescription(id: ClusterId): Option[MutableClusterDescription] = withIntegrity {
    this.dao.getDescription(id)
  }

  def registerNewCluster(id: ClusterId, name: String, size: Int): MutableClusterDescription = {
    val description = registerCluster(id, name, size)
    description.state = Provisioning
    description
  }

  def registerNewCluster(name: String, size: Int): MutableClusterDescription =
    registerCluster(ClusterId(), name, size)

  override def registerCluster(id: ClusterId, name: String, size: Int): MutableClusterDescription =
    dao.registerCluster(id, name, size)

  override def getUsers(id: ClusterId): Option[Set[ClusterUser]] = dao.getUsers(id)

  override def setUsers(id: ClusterId, users: Set[ClusterUser]) = dao.setUsers(id, users)

  private def verifyClusterIntegrity(id: ClusterId): Future[Unit] = {
    val daoCluster = this.dao.getDescription(id).get
    for {
      ambariCluster <- ambariServer.getCluster(id.toString)
      ambariState <- resolveState(ambariCluster, allServices)
      daoState = daoCluster.state
    } yield (daoState, ambariState) match {
      case (_, AmbariClusterState.ClusterNotPresent) => daoCluster.state = Terminated
      case (Terminating, AmbariClusterState.Running) => daoCluster.state = Running
      case (Running, AmbariClusterState.Unknown) => daoCluster.state = Failed(
        "Mismatch between SQL and Ambari information")
      case (Provisioning, AmbariClusterState.Running) => daoCluster.state = Running
      case _ => ()
    }
  }

  private def verifyIntegrity = {
    val clusterIds = dao.ids
    for {
      clusterNames <- ambariServer.listClusterNames
      (daoClustersInAmbari, daoClustersNotInAmbari) = clusterIds.partition(id =>
        clusterNames.contains(id.toString))
      _ <- Future.traverse(daoClustersInAmbari)(verifyClusterIntegrity)
    } yield {
      daoClustersNotInAmbari.foreach(id => dao.getDescription(id).map(_.state = Terminated))
    }
  }

  private val initialIntegrityValidation = verifyIntegrity

  private def withIntegrity[T](body: => T) = {
    Await.result(initialIntegrityValidation, initializationPeriod)
    body
  }
}

