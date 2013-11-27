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

package es.tid.cosmos.servicemanager

import scala.concurrent.Future

import es.tid.cosmos.platform.common.ExecutableValidation
import es.tid.cosmos.servicemanager.clusters.{ClusterId, ImmutableClusterDescription}

/**
 * Cluster manager that allows cluster creation, termination as well as
 * querying about the state of a managed cluster.
 */
trait ServiceManager {

  type ServiceDescriptionType <: ServiceDescription

  /**
   * Get the IDs of the existing clusters managed by this manager.
   * @return the IDs of the existing clusters
   */
  def clusterIds: Seq[ClusterId]

  /**
   * A sequence of all services this service manager supports
   */
  val services: Seq[ServiceDescriptionType]

  /**
   * Create a cluster of a given size with a specified set of services.
   *
   * @param name the cluster's name
   * @param clusterSize the number of nodes the cluster should comprise of
   * @param serviceDescriptions the description of services to be installed to the cluster
   * @param users the list of users the cluster should have
   * @return the ID of the newly created cluster
   */
  def createCluster(
    name: String,
    clusterSize: Int,
    serviceDescriptions: Seq[ServiceDescriptionType],
    users: Seq[ClusterUser],
    preConditions: ExecutableValidation): ClusterId

  /**
   * Obtain information of an existing cluster's state.
   *
   * @param id the ID of the cluster
   * @return the description of the cluster and it state iff found
   */
  def describeCluster(id: ClusterId): Option[ImmutableClusterDescription]

  /**
   * Terminate an existing cluster.
   *
   * @param id the ID of the cluster to terminate
   */
  def terminateCluster(id: ClusterId): Future[Unit]

  /**
   * The cluster id of the persistent HDFS cluster
   */
  def persistentHdfsId: ClusterId

  /**
   * Deploys the persistent HDFS cluster.
   */
  def deployPersistentHdfsCluster(): Future[Unit]

  /**
   * Obtain information of the persistent HDFS cluster's state.
   */
  def describePersistentHdfsCluster(): Option[ImmutableClusterDescription]

  /**
   * Terminates the persistent HDFS cluster.
   */
  def terminatePersistentHdfsCluster(): Future[Unit]

  /**
   * Add users to the CosmosUser service for the given cluster.
   *
   * @param clusterId the cluster id where the users will be added
   * @param users the users to be added to the CosmosUser service
   */
  def setUsers(clusterId: ClusterId, users: Seq[ClusterUser]): Future[Unit]

  /**
   * Get the total number of cluster nodes managed by the service manager.
   *
   * @return the total number of nodes regardless of their state and usage
   */
  def clusterNodePoolCount: Int
}
