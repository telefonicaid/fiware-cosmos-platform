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

package es.tid.cosmos.api.profile

import java.util.Date

import es.tid.cosmos.servicemanager.clusters.ClusterId

trait ClusterDao[Conn] {

  /** Retrieves the set of cluster ids for a given user.
    *
    * @param id  The unique Cosmos ID of the given user.
    * @param c   The connection to use.
    * @return    The set of assigned clusters for a given user.
    * @throws CosmosProfileException  When no user has such id
    */
  def ownedBy(id: ProfileId)(implicit c: Conn): Seq[ClusterAssignment]

  /** Determines the owner of a cluster if any.
    *
    * @param clusterId  Id of the cluster whose ownership is looked for
    * @param c          The connection to use
    * @return           The owner profile id or none
    */
  def ownerOf(clusterId: ClusterId)(implicit c: Conn): Option[ProfileId]

  /** Assigns a cluster to a given user.
    *
    * @param assignment  Assignment to make persistent.
    * @param c           The connection to use.
    */
  def assignCluster(assignment: ClusterAssignment)(implicit c: Conn): Unit

  /** Assigns a cluster to a given user at the present moment.
    * @param clusterId  The cluster ID to assign.
    * @param ownerId    The unique Cosmos ID of the new owner.
    * @param c          The connection to use.
    */
  def assignCluster(clusterId: ClusterId, ownerId: Long)(implicit c: Conn) {
    assignCluster(ClusterAssignment(clusterId, ownerId, new Date()))
  }

  /** Assigns a cluster to a given user.
    *
    * @param clusterId     The cluster ID to assign.
    * @param ownerId       The unique Cosmos ID of the new owner.
    * @param creationDate  The instant the cluster creation started.
    * @param c             The connection to use.
    */
  def assign(clusterId: ClusterId, ownerId: Long, creationDate: Date)(implicit c: Conn) {
    assignCluster(ClusterAssignment(clusterId, ownerId, creationDate))
  }
}
