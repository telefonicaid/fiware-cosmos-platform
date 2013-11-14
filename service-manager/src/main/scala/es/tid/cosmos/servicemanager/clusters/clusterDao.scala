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

trait ClusterDaoComponent {
  val clusterDao: ClusterDao
}

/**
  * A DAO that stores information about clusters
  */
trait ClusterDao {
  /**
    * @return The list of cluster IDs this DAO is aware of
    */
  def ids: Seq[ClusterId]

  /**
    * Gets the description of a cluster
    *
    * @param id The id of the cluster for which we want a description
    * @return An optional representation of a cluster which can be mutated. Mutations
    *         on this object will affect the state of the DAO.
    */
  def getDescription(id: ClusterId): Option[MutableClusterDescription]

  /**
    * Adds a new cluster to the DAO
    * @param id The id of the cluster
    * @param name The name of the cluster
    * @param size The size of the cluster
    * @return A representation of a cluster which can be mutated. Mutations
    *         on this object will affect the state of the DAO.
    */
  def registerCluster(id: ClusterId, name: String, size: Int): MutableClusterDescription
}
