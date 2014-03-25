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

package es.tid.cosmos.api.profile.dao

import java.util.Date
import java.sql.Timestamp

import es.tid.cosmos.api.profile._
import es.tid.cosmos.servicemanager.clusters.ClusterId

trait ClusterDao[Conn] {

  /** Retrieves the cluster assignments for a given cluster
    *
    * @param id The id of the cluster
    * @param c  The connection to use
    * @return   The cluster assignment
    * @throws CosmosDaoException When no cluster with the specified id exists
    */
  def get(id: ClusterId)(implicit c: Conn): Cluster

  /** Retrieves the set of clusters for a given user.
    *
    * @param id  The unique Cosmos ID of the given user
    * @param c   The connection to use
    * @return    The set of assigned clusters for a given user
    * @throws CosmosDaoException  When no user has such id
    */
  def ownedBy(id: ProfileId)(implicit c: Conn): Seq[Cluster]

  /** Retrieves a cluster by its secret.
    *
    * @param secret  Secret of the cluster to look for
    * @param c       The connection to use
    * @return        A Cluster or None
    */
  def lookupBySecret(secret: ClusterSecret)(implicit c: Conn): Option[Cluster]

  /** Determines the owner of a cluster if any.
    *
    * @param clusterId  Id of the cluster whose ownership is looked for
    * @param c          The connection to use
    * @return           The owner profile id or none
    */
  def ownerOf(clusterId: ClusterId)(implicit c: Conn): Option[ProfileId]

  /** Assigns a cluster to a given user.
    *
    * @param cluster  Cluster to persist, must have a defined cluster secret
    * @param c        The connection to use
    * @return         Newly registered cluster
    */
  def register(cluster: Cluster)(implicit c: Conn): Cluster

  /** Assigns a cluster to a given user at the present moment.
    *
    * @param clusterId  The cluster ID to assign
    * @param ownerId    The unique Cosmos ID of the new owner
    * @param c          The connection to use
    * @param secret     The cluster secret for Infinity access
    * @param shared     Whether the cluster is shared or not
    * @return           Newly registered cluster
    */
  def register(clusterId: ClusterId, ownerId: Long, secret: ClusterSecret, shared: Boolean = false)
              (implicit c: Conn): Cluster = register(Cluster(
    clusterId,
    ownerId,
    creationDate = new Timestamp(new Date().getTime),
    shared,
    Some(secret)
  ))
}
