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

package es.tid.cosmos.api.profile.dao.sql

import java.sql.{Timestamp, Connection}

import anorm._
import anorm.SqlParser._

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.dao.ClusterDao
import es.tid.cosmos.servicemanager.clusters.ClusterId

private[sql] object PlayDbClusterDao extends ClusterDao[Connection] {

  override def get(clusterId: ClusterId)(implicit c: Connection): Cluster =
    asCluster(SQL(
      """SELECT cluster_id, owner, creation_date, shared, cluster_secret
        |FROM cluster WHERE cluster_id = {cluster_id}""".stripMargin)
      .on("cluster_id" -> clusterId.id)
      .single())

  override def ownedBy(id: ProfileId)(implicit c: Connection): Seq[Cluster] = SQL(
    """SELECT cluster_id, owner, creation_date, shared, cluster_secret
      |FROM cluster WHERE owner = {owner}""".stripMargin
  ).on("owner" -> id)
    .apply()
    .collect(asCluster)
    .force

  override def ownerOf(clusterId: ClusterId)(implicit c: Connection): Option[ProfileId] =
    SQL("SELECT owner FROM cluster WHERE cluster_id = {cluster_id}")
      .on("cluster_id" -> clusterId.id)
      .as(scalar[ProfileId].singleOpt)

  override def register(cluster: Cluster)(implicit c: Connection): Cluster = {
    require(cluster.secret.isDefined, "Missing cluster secret")
    SQL("""INSERT INTO cluster(cluster_id, owner, creation_date, shared, cluster_secret)
          | VALUES ({cluster_id}, {owner}, {creation_date}, {shared}, {cluster_secret})
        """.stripMargin).on(
        "cluster_id" -> cluster.clusterId.toString,
        "owner" -> cluster.ownerId,
        "creation_date" -> cluster.creationDate.toString,
        "shared" -> cluster.shared,
        "cluster_secret" -> cluster.secret.get.underlying
      ).execute()
    cluster
  }

  override def lookupBySecret(secret: ClusterSecret)(implicit c: Connection): Option[Cluster] =
    SQL("""SELECT cluster_id, owner, creation_date, shared, cluster_secret
          |FROM cluster WHERE cluster_secret = {cluster_secret}""".stripMargin)
      .on("cluster_secret" -> secret.underlying)
      .apply()
      .collectFirst(asCluster)

  private val asCluster: PartialFunction[Row, Cluster] = {
    case Row(
      clusterId: String,
      ownerId: Int,
      creationDate: Timestamp,
      shared: Int,
      secret: Option[_]
    ) => Cluster(
      ClusterId(clusterId),
      ownerId,
      creationDate,
      shared != 0,
      secret.asInstanceOf[Option[String]].map(ClusterSecret.apply)
    )
  }
}
