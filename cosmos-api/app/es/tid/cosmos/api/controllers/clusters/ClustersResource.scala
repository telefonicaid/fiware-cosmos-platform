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

package es.tid.cosmos.api.controllers.clusters

import java.util.Date
import scala.util.{Failure, Success, Try}

import com.wordnik.swagger.annotations._
import play.Logger
import play.api.libs.json._
import play.api.mvc._

import es.tid.cosmos.api.controllers._
import es.tid.cosmos.api.controllers.common.{Message, AuthController, JsonController}
import es.tid.cosmos.api.controllers.pages.CosmosProfile
import es.tid.cosmos.api.profile.{ClusterAssignment, CosmosProfileDao}
import es.tid.cosmos.servicemanager.{ServiceManager, ClusterId}

/**
 * Resource that represents the whole set of clusters.
 */
@Api(value = "/cosmos/v1/clusters", listingPath = "/doc/cosmos/v1/clusters",
  description = "Represents all the clusters in the platform")
class ClustersResource(serviceManager: ServiceManager, override val dao: CosmosProfileDao)
  extends JsonController with AuthController {
  /**
   * List user clusters.
   */
  @ApiOperation(value = "List clusters", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.clusters.ClusterList")
  def list = Action { implicit request =>
    Authenticated(request) { profile =>
      Ok(Json.toJson(ClusterList(listClusters(profile).map(_.withAbsoluteUri(request)))))
    }
  }

  /**
   * Start a new cluster provisioning.
   */
  @ApiOperation(value = "Create a new cluster", httpMethod = "POST")
  @ApiErrors(Array(
    new ApiError(code = 400, reason = "Invalid JSON payload"),
    new ApiError(code = 403, reason = "Quota exceeded")
  ))
  @ApiParamsImplicit(Array(
    new ApiParamImplicit(paramType = "body",
      dataType = "es.tid.cosmos.api.controllers.clusters.CreateClusterParams")
  ))
  def createCluster = JsonBodyAction[CreateClusterParams] { (request, body) =>
    Authenticated(request) { profile =>
      if (profile.quota.withinQuota(body.size + usedMachines(profile))) {
        val services = serviceManager.services.filter(
          service => body.optionalServices.contains(service.name))
        Try(serviceManager.createCluster(
          body.name, body.size, services, Seq(profile.toClusterUser))) match {
          case Failure(ex) => throw ex
          case Success(clusterId: ClusterId) => {
            Logger.info(s"Provisioning new cluster $clusterId")
            val assignment = ClusterAssignment(clusterId, profile.id, new Date())
            dao.withTransaction { implicit c => dao.assignCluster(assignment) }
            val clusterDescription = serviceManager.describeCluster(clusterId).get
            val reference = ClusterReference(clusterDescription, assignment).withAbsoluteUri(request)
            Created(Json.toJson(reference)).withHeaders(LOCATION -> reference.href)
          }
        }
      } else Forbidden(Json.toJson(Message("Quota exceeded")))
    }
  }

  private def listClusters(profile: CosmosProfile) = {
    val assignedClusters = dao.withConnection { implicit c =>
      Set(dao.clustersOf(profile.id): _*)
    }
    (for {
      assignment <- assignedClusters.toList
      description <- serviceManager.describeCluster(assignment.clusterId).toList
    } yield ClusterReference(description, assignment)).sorted(ClustersDisplayOrder)
  }

  private def usedMachines(profile: CosmosProfile) = listClusters(profile).map(_.description.size).sum
}
