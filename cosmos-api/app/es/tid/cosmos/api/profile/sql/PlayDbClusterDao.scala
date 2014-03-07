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

package es.tid.cosmos.api.profile.sql

import java.sql.Connection
import java.util.Date

import anorm._
import anorm.SqlParser._

import es.tid.cosmos.api.profile._
import es.tid.cosmos.servicemanager.clusters.ClusterId
import es.tid.cosmos.api.profile.ClusterAssignment

object PlayDbClusterDao extends ClusterDao[Connection] {

  override def ownedBy(id: ProfileId)(implicit c: Connection): Seq[ClusterAssignment] =
    SQL("SELECT cluster_id, creation_date FROM cluster WHERE owner = {owner}")
      .on("owner" -> id)
      .apply()
      .collect({
      case Row(clusterId: String, creationDate: Date) =>
        ClusterAssignment(ClusterId(clusterId), id, creationDate)
    }).force

  override def ownerOf(clusterId: ClusterId)(implicit c: Connection): Option[ProfileId] =
    SQL("SELECT owner FROM cluster WHERE cluster_id = {cluster_id}")
      .on("cluster_id" -> clusterId.id)
      .as(scalar[ProfileId].singleOpt)

  override def register(assignment: ClusterAssignment)(implicit c: Connection): Unit =
    SQL("""INSERT INTO cluster(cluster_id, owner, creation_date)
          | VALUES ({cluster_id}, {owner}, {creation_date})""".stripMargin).on(
        "cluster_id" -> assignment.clusterId.toString,
        "owner" -> assignment.ownerId,
        "creation_date" -> assignment.creationDate
      ).execute()
}
