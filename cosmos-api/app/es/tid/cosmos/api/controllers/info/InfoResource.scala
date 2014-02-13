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

package es.tid.cosmos.api.controllers.info

import com.wordnik.swagger.annotations._
import play.api.libs.json.Json
import play.api.mvc.{Controller, Action}

import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.quota.Group
import es.tid.cosmos.servicemanager.ServiceManager

@Api(value = "/cosmos/v1/info", listingPath = "/doc/cosmos/v1/info",
  description = "Provides general-purpose information about a platform user")
class InfoResource(override val dao: CosmosProfileDao, serviceManager: ServiceManager)
  extends Controller with JsonController with ApiAuthController {

  @ApiOperation(value = "Get general information about the user whose credentials are used",
    httpMethod = "GET", responseClass = "es.tid.cosmos.api.controllers.info.Info")
  @ApiErrors(Array(new ApiError(code = 401, reason = "Unauthorized user")))
  def get = Action { implicit request =>
    for {
      profile <- requireAuthenticatedApiRequest(request)
    } yield Ok(Json.toJson(gatherInfo(profile)))
  }

  private def gatherInfo(profile: CosmosProfile) = dao.withTransaction { implicit c =>
    Info(
      profileId = profile.id,
      handle = profile.handle,
      individualQuota = profile.quota.toOptInt,
      group = groupInfo(profile.group),
      clusters = clustersInfo(dao.clustersOf(profile.id), profile),
      resources = resourcesInfo(profile)
    )
  }

  private def clustersInfo(userClusters: Seq[ClusterAssignment], profile: CosmosProfile) = {
    val ownClusters = userClusters.map(_.clusterId)
    val accessibleClusters = for {
      clusterId <- serviceManager.clusterIds
      description <- serviceManager.describeCluster(clusterId)
      users <- description.users
      if users.exists(_.username == profile.handle)
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

  private def currentQuotaContext() =
    new QuotaContextFactory(new CosmosMachineUsageDao(dao, serviceManager)).apply()
}
