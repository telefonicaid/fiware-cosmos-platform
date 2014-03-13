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

import java.sql.Timestamp

import es.tid.cosmos.servicemanager.clusters.ClusterId

/** Cluster owned by a user profile.
  *
  * @param clusterId     Owned cluster
  * @param ownerId       Cluster creator
  * @param creationDate  Creation timestamp
  * @param shared        Whether the cluster is shared with the user group
  * @param secret        Cluster secret or None for legacy clusters
  */
case class Cluster(
    clusterId: ClusterId,
    ownerId: Long,
    creationDate: Timestamp,
    shared: Boolean = false,
    secret: Option[ClusterSecret] = None
)
