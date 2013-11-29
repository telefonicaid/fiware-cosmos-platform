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

package es.tid.cosmos.api.controllers.admin.stats

import play.api.libs.json.{JsValue, Json, Writes}

case class ClustersStats(clusters: Seq[ClusterStats])

object ClustersStats {
  implicit val clusterStatsWrites: Writes[ClustersStats] = new Writes[ClustersStats] {
    override def writes(stats: ClustersStats): JsValue = Json.obj(
      "clusters" -> stats.clusters
    )
  }
}
