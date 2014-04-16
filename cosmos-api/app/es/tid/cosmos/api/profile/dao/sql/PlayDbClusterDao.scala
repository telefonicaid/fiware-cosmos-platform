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
