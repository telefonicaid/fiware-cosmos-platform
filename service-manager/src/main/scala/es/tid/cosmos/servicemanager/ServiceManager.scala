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

package es.tid.cosmos.servicemanager

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future

import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.services.Service
import es.tid.cosmos.servicemanager.services.InfinityServer.InfinityServerParameters

/**
 * Cluster manager that allows cluster creation, termination as well as
 * querying about the state of a managed cluster.
 */
trait ServiceManager {

  /** Get the IDs of the existing clusters managed by this manager.
    * @return the IDs of the existing clusters
    */
  def clusterIds: Seq[ClusterId]

  /** A sequence of all services this service manager supports. */
  val optionalServices: Set[Service]

  /** Create a cluster of a given size with a specified set of services.
    *
    * @param name          the cluster's name
    * @param clusterSize   the number of nodes the cluster should comprise of
    * @param services      the concrete service instances to be installed to the cluster
    * @param users         the list of users the cluster should have
    * @param preConditions the pre-conditions to be validated before attempting to create a cluster
    * @return              the ID of the newly created cluster
    */
  def createCluster(
    name: ClusterName,
    clusterSize: Int,
    services: Set[AnyServiceInstance],
    users: Seq[ClusterUser],
    preConditions: ClusterExecutableValidation = UnfilteredPassThrough): ClusterId

  /** Obtain information of an existing cluster's state.
    *
    * @param id the ID of the cluster
    * @return the description of the cluster and it state iff found
    */
  def describeCluster(id: ClusterId): Option[ImmutableClusterDescription]

  /** Terminate an existing cluster.
    *
    * @param id the ID of the cluster to terminate
    */
  def terminateCluster(id: ClusterId): Future[Unit]

  /** The cluster id of the persistent HDFS cluster */
  def persistentHdfsId: ClusterId

  /* Deploys the persistent HDFS cluster. */
  def deployPersistentHdfsCluster(parameters: InfinityServerParameters): Future[Unit]

  /** A convenience function to obtain information of the persistent HDFS cluster's state. */
  final def describePersistentHdfsCluster(): Option[ImmutableClusterDescription] =
    describeCluster(persistentHdfsId)

  /** A convenience function to terminate the persistent HDFS cluster. */
  final def terminatePersistentHdfsCluster(): Future[Unit] = terminateCluster(persistentHdfsId)

  /** List the users of a cluster.
    *
    * @param clusterId the cluster id which users are listed
    * @return the sequence of users of the cluster
    */
  def listUsers(clusterId: ClusterId): Option[Seq[ClusterUser]]

  /**
   * Set the users of the CosmosUser service for the given cluster.
   *
   * @param clusterId the cluster id where the users will be added
   * @param users the users to be added to the CosmosUser service
   */
  def setUsers(clusterId: ClusterId, users: Seq[ClusterUser]): Future[Unit]

  /** A convenience function to add a new user to a cluster.
    *
    * This convenience function may be used to add a new user to an existing cluster. The list
    * of users of the cluster must be available before invoking this function. Otherwise a failure
    * is returned with a embed IllegalStateException. If there is a user with the same username
    * already defined it will be replaced.
    */
  def addUser(clusterId: ClusterId, user: ClusterUser): Future[Unit] = {
    listUsers(clusterId) match {
      case None =>
        Future.failed(new IllegalStateException(
          s"cannot add user to cluster $clusterId: user list is not available yet"))
      case Some(currentUsers) =>
        val newUsers = currentUsers.filterNot(_.username.equals(user.username)) :+ user
        setUsers(clusterId, newUsers)
    }
  }

  /** A convenience function to disable a user from a cluster.
    *
    * This convenience function may be used to disable a user from a cluster. The user is
    * not removed but instead his SSH and HDFS permissions on that cluster are revoked. The
    * list of users of the cluster must be available before invoking this function. Otherwise
    * a failure is returned with a embed IllegalStateException.
    */
  def disableUser(clusterId: ClusterId, username: String): Future[Unit] = {
    listUsers(clusterId) match {
      case None =>
        Future.failed(new IllegalStateException(
          s"cannot disable user from cluster $clusterId: user list is not available yet"))
      case Some(currentUsers) =>
        val newUsers = currentUsers.map(usr =>
          if (usr.username.equals(username)) ClusterUser.disabled(username, usr.publicKey)
          else usr
        )
        setUsers(clusterId, newUsers)
    }
  }

  /** Disable the given user from all clusters in running state whose user list is available. */
  def disableUserFromAll(username: String): Future[Unit] = {
    val runningClusters = clusterIds
      .flatMap(id => describeCluster(id))
      .filter(c => c.state.equals(Running))
    val clustersWithUsers = runningClusters.flatMap(c => listUsers(c.id).map(_ => c))
    Future.sequence(clustersWithUsers.map(c => disableUser(c.id, username))).map(_ => ())
  }

  /** Get the total number of cluster nodes managed by the service manager.
    *
    * @return the total number of nodes regardless of their state and usage
    */
  def clusterNodePoolCount: Int
}
