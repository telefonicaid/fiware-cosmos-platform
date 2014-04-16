/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.servicemanager.ambari

import scala.concurrent.{Await, Future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._
import scala.language.postfixOps

import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}
import es.tid.cosmos.servicemanager.ambari.services.AmbariService
import es.tid.cosmos.servicemanager.ambari.rest.AmbariServer
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.services.Service

/** This class wraps a ClusterDao and makes sure that the data in Ambari and the data in the
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
    allServices: Set[AmbariService],
    initializationPeriod: FiniteDuration = 5 minutes)
  extends ClusterDao with ClusterStateResolver {

  override def ids: Seq[ClusterId] = withIntegrity {
    this.dao.ids
  }

  override def getDescription(id: ClusterId): Option[MutableClusterDescription] = withIntegrity {
    this.dao.getDescription(id)
  }

  override def registerCluster(
    id: ClusterId = ClusterId.random(),
    name: ClusterName,
    size: Int,
    services: Set[Service]): MutableClusterDescription =
    dao.registerCluster(id, name, size, services)

  override def getUsers(id: ClusterId): Option[Set[ClusterUser]] = dao.getUsers(id)

  override def setUsers(id: ClusterId, users: Set[ClusterUser]) = dao.setUsers(id, users)

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

  private val initialIntegrityValidation = verifyIntegrity

  private def withIntegrity[T](body: => T) = {
    Await.result(initialIntegrityValidation, initializationPeriod)
    body
  }
}

