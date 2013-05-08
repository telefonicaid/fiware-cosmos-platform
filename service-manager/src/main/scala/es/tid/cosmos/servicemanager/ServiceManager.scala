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
  def services: Seq[ServiceDescriptionType]

  /**
   * Create a cluster of a given size with a specified set of services.
   *
   * @param name the cluster's name
   * @param clusterSize the number of nodes the cluster should comprise of
   * @param serviceDescriptions the description of services to be installed to the cluster
   * @return the ID of the newly created cluster
   */
  def createCluster(
    name: String, clusterSize: Int, serviceDescriptions: Seq[ServiceDescriptionType]): ClusterId

  /**
   * Obtain information of an existing cluster's state.
   *
   * @param id the ID of the cluster
   * @return the description of the cluster and it state iff found
   */
  def describeCluster(id: ClusterId): Option[ClusterDescription]

  /**
   * Terminate an existing cluster.
   *
   * @param id the ID of the cluster to terminate
   */
  def terminateCluster(id: ClusterId): Future[Unit]
}
