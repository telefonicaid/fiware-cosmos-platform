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
