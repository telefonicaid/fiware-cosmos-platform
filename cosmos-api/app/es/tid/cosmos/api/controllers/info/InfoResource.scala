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

package es.tid.cosmos.api.controllers.info

import com.wordnik.swagger.annotations._
import play.api.libs.json.Json
import play.api.mvc.{Controller, Action}

import es.tid.cosmos.api.auth.request.RequestAuthentication
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.common.auth.ApiAuthController
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.dao.{ClusterDataStore, ClusterDao, DataStore}
import es.tid.cosmos.api.quota.Group
import es.tid.cosmos.api.usage.MachineUsage
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.{ClusterDao => _, _}

@Api(value = "/cosmos/v1/info", listingPath = "/doc/cosmos/v1/info",
  description = "Provides general-purpose information about a platform user")
class InfoResource(
     override val auth: RequestAuthentication,
     store: ClusterDataStore,
     serviceManager: ServiceManager,
     machineUsage: MachineUsage) extends Controller with JsonController with ApiAuthController {

  @ApiOperation(value = "Get general information about the user whose credentials are used",
    httpMethod = "GET", responseClass = "es.tid.cosmos.api.controllers.info.Info")
  @ApiErrors(Array(new ApiError(code = 401, reason = "Unauthorized user")))
  def get = Action { implicit request =>
    for {
      profile <- requireAuthenticatedApiRequest(request)
    } yield Ok(Json.toJson(gatherInfo(profile)))
  }

  private def gatherInfo(profile: CosmosProfile) = store.withTransaction { implicit c =>
    val userClusters = store.cluster.ownedBy(profile.id).map(_.clusterId)
    Info(
      profileId = profile.id,
      handle = profile.handle,
      individualQuota = profile.quota.toOptInt,
      group = groupInfo(profile.group),
      clusters = clustersInfo(userClusters, profile),
      resources = resourcesInfo(profile)
    )
  }

  private def clustersInfo(userClusters: Seq[ClusterId], profile: CosmosProfile) = {
    val ownClusters = for {
      clusterId <- userClusters
      description <- serviceManager.describeCluster(clusterId)
      if description.state != Terminated
    } yield clusterId
    val accessibleClusters = for {
      clusterId <- serviceManager.clusterIds
      description <- serviceManager.describeCluster(clusterId)
      if description.state == Running
      users <- description.users
      if users.exists(user => user.username == profile.handle && user.sshEnabled)
    } yield clusterId
    ClustersInfo(ownClusters, accessibleClusters)
  }

  private def groupInfo(group: Group) = GroupInfo(
    name = group.name,
    guaranteedQuota = group.minimumQuota.toOptInt
  )

  private def resourcesInfo(profile: CosmosProfile): ResourcesInfo = {
    val context = currentQuotaContext()
    ResourcesInfo(
      groupConsumption = context.usageByGroup(profile.group),
      individualConsumption = context.usageByProfile(profile.id),
      available = context.available,
      availableForGroup = context.availableForGroup(profile.group),
      availableForUser = context.availableForUser(profile)
    )
  }

  private def currentQuotaContext() = new QuotaContextFactory(machineUsage).apply()
}
