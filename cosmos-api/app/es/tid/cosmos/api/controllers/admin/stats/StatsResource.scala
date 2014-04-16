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

package es.tid.cosmos.api.controllers.admin.stats

import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import com.wordnik.swagger.annotations.{ApiError, ApiErrors, ApiOperation}
import play.api.libs.json.Json
import play.api.mvc._

import es.tid.cosmos.api.auth.request.RequestAuthentication
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.common.auth.ApiAuthController
import es.tid.cosmos.api.profile.dao._
import es.tid.cosmos.platform.ial.InfrastructureProvider
import es.tid.cosmos.platform.ial.MachineProfile
import es.tid.cosmos.platform.ial.MachineProfile.MachineProfile
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.{ClusterState, ClusterId}

/** Stats for administration purposes */
class StatsResource(
    override val auth: RequestAuthentication,
    store: ProfileDataStore with ClusterDataStore,
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

  private def ownerHandleOf(clusterId: ClusterId): String = store.withTransaction { implicit c =>
    for {
      cosmosId <- store.cluster.ownerOf(clusterId)
      profile <- store.profile.lookupByProfileId(cosmosId)
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
