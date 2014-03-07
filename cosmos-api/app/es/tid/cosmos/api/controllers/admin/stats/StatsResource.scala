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

import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import com.wordnik.swagger.annotations.{ApiError, ApiErrors, ApiOperation}
import play.api.libs.json.Json
import play.api.mvc._

import es.tid.cosmos.api.auth.request.RequestAuthentication
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.common.auth.ApiAuthController
import es.tid.cosmos.api.profile.CosmosDao
import es.tid.cosmos.platform.ial.InfrastructureProvider
import es.tid.cosmos.platform.ial.MachineProfile
import es.tid.cosmos.platform.ial.MachineProfile.MachineProfile
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.{ClusterState, ClusterId}

/** Stats for administration purposes */
class StatsResource(
    override val auth: RequestAuthentication,
    dao: CosmosDao,
    serviceManager: ServiceManager,
    ial: InfrastructureProvider
  ) extends Controller with ApiAuthController {

  @ApiOperation(value = "List active clusters of all users", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.admin.stats.ClustersStats")
  @ApiErrors(Array(
    new ApiError(code = 401, reason = "Unauthorized request"),
    new ApiError(code = 403, reason = "Forbidden for non-operators")
  ))
  def listActiveClusters = Action { request =>
    for {
      _ <- requireOperatorApiRequest(request)
    } yield Ok(Json.toJson(ClusterStatsSet(activeClusterStats.toSet)))
  }

  private def activeClusterStats = for {
    clusterId <- serviceManager.clusterIds
    description <- serviceManager.describeCluster(clusterId)
    if ClusterState.ActiveStates.contains(description.state)
    ownerHandle = ownerHandleOf(clusterId)
  } yield ClusterStats(
    id = clusterId.toString,
    name = description.name.underlying,
    ownerHandle = ownerHandle,
    size = description.size,
    master = description.master.map(_.hostname).getOrElse(""),
    slaves = description.slaves.map(_.hostname)
  )

  private def ownerHandleOf(clusterId: ClusterId): String = dao.withTransaction { implicit c =>
    for {
      cosmosId <- dao.cluster.ownerOf(clusterId)
      profile <- dao.profile.lookupByProfileId(cosmosId)
    } yield profile.handle
  }.getOrElse(StatsResource.UnknownOwnerHandle)

  @ApiOperation(value = "Total and available machines by profile", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.admin.stats.MachineStats")
  @ApiErrors(Array(
    new ApiError(code = 401, reason = "Unauthorized request"),
    new ApiError(code = 403, reason = "Forbidden for non-operators")
  ))
  def listMachinesByProfile = Action.async { request =>
    for {
      _ <- requireOperatorApiRequest(request)
    } yield pollMachineStats
  }

  private def pollMachineStats: Future[SimpleResult] = {
    val uses_> = Future.sequence(MachineProfile.values.toSeq.map { profile =>
      for {
        usage <- usageForProfile(profile)
      } yield profile -> usage
    }).map(_.toMap)
    for {
      uses <- uses_>
    } yield Ok(Json.toJson(MachineStats(uses)))
  }

  private def usageForProfile(profile: MachineProfile): Future[ResourceUse] = for {
    available <- ial.availableMachineCount(profile)
    total = ial.machinePoolCount(p => p == profile)
  } yield ResourceUse(total, available)
}

object StatsResource {
  val UnknownOwnerHandle: String = "<unknown>"
}
