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

import play.api.libs.json.{Writes, Json}

case class ClusterStats(
    id: String,
    name: String,
    ownerHandle: String,
    size: Int)

object ClusterStats {
  implicit val clusterStatWrites: Writes[ClusterStats] = Json.writes[ClusterStats]
}
