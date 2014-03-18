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

package es.tid.cosmos.servicemanager.clusters

import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}
import es.tid.cosmos.servicemanager.services.Service

trait ClusterDaoComponent {
  def serviceManagerClusterDao: ClusterDao
}

/** A DAO that stores information about clusters. */
trait ClusterDao {

  /** The list of cluster IDs this DAO is aware of. */
  def ids: Seq[ClusterId]

  /** Gets the description of a cluster
    *
    * @param id The id of the cluster for which we want a description
    * @return An optional representation of a cluster which can be mutated. Mutations
    *         on this object will affect the state of the DAO.
    */
  def getDescription(id: ClusterId): Option[MutableClusterDescription]

  /** Adds a new cluster to the DAO
    * @param id The id of the cluster
    * @param name The name of the cluster
    * @param size The size of the cluster
    * @param services the enabled services of the cluster
    * @return A representation of a cluster which can be mutated. Mutations
    *         on this object will affect the state of the DAO.
    */
  def registerCluster(
    id: ClusterId = ClusterId.random(),
    name: ClusterName,
    size: Int,
    services: Set[Service]): MutableClusterDescription

  /** Retrieve the list of users for given cluster.
    *
    * @param id The id of the cluster
    * @return The sequence of users for that cluster or none if cluster doesn't exist
    */
  def getUsers(id: ClusterId): Option[Set[ClusterUser]]

  /** Set the users for given cluster.
    *
    * @param id The id of the cluster
    * @param users The new user seq for the cluster
    * @throws IllegalArgumentException If there is no cluster with given id
    */
  def setUsers(id: ClusterId, users: Set[ClusterUser]): Unit
}
