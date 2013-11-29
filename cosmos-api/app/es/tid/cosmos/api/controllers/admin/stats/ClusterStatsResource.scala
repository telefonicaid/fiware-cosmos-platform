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

import com.wordnik.swagger.annotations.{ApiError, ApiErrors, ApiOperation}
import play.api.mvc._
import play.api.libs.json.Json

import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile.CosmosProfileDao
import es.tid.cosmos.servicemanager.ServiceManager

/** Cluster stats for administration purposes */
class ClusterStatsResource(
    override val dao: CosmosProfileDao,
    serviceManager: ServiceManager
  ) extends Controller with ApiAuthController {

  @ApiOperation(value = "List active clusters of all users", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.admin.stats.ClusterStats")
  @ApiErrors(Array(
    new ApiError(code = 401, reason = "Unauthenticated request"),
    new ApiError(code = 403, reason = "Forbidden for non-operators")
  ))
  def list = Action { implicit request =>
    for {
      profile <- requireOperatorApiRequest(request)
    } yield Ok(Json.toJson(ClustersStats(listActiveClusters())))
  }

  private def listActiveClusters() = for {
    clusterId <- serviceManager.clusterIds
    description <- serviceManager.describeCluster(clusterId)
    ownerHandle = dao.withTransaction { implicit c =>
      for {
        cosmosId <- dao.ownerOf(clusterId)
        profile <- dao.lookupByProfileId(cosmosId)
      } yield profile.handle
    }.getOrElse("<unknown>")
  } yield ClusterStats(
    id = clusterId.toString,
    name = description.name,
    ownerHandle = ownerHandle,
    size = description.size
  )
}
