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

import scala.util.{Failure, Success, Try}

import com.wordnik.swagger.annotations._
import play.Logger
import play.api.Play.current
import play.api.db.DB
import play.api.libs.json._
import play.api.mvc._

import es.tid.cosmos.api.controllers.common.{AuthController, JsonController}
import es.tid.cosmos.api.profile.CosmosProfileDao
import es.tid.cosmos.servicemanager.{ServiceManager, ClusterId}

/**
 * Resource that represents the whole set of clusters.
 */
@Api(value = "/cosmos/v1/clusters", listingPath = "/doc/cosmos/v1/clusters",
  description = "Represents all the clusters in the platform")
class ClustersResource(serviceManager: ServiceManager) extends JsonController with AuthController {
  /**
   * List user clusters.
   */
  @ApiOperation(value = "List clusters", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.clusters.ClusterList")
  def list = Action { implicit request =>
    Authenticated(request) { profile =>
      val userClusters = DB.withConnection { implicit c =>
        Set(CosmosProfileDao.clustersOf(profile.id): _*)
      }
      val clusters = serviceManager.clusterIds.filter(userClusters).toList.sorted
      val body = ClusterList(clusters.map(id => ClusterReference(id)))
      Ok(Json.toJson(body))
    }
  }

  /**
   * Start a new cluster provisioning.
   */
  @ApiOperation(value = "Create a new cluster", httpMethod = "POST")
  @ApiErrors(Array(
    new ApiError(code = 400, reason = "Invalid JSON payload")
  ))
  @ApiParamsImplicit(Array(
    new ApiParamImplicit(paramType = "body",
      dataType = "es.tid.cosmos.api.controllers.clusters.CreateClusterParams")
  ))
  def createCluster = JsonBodyAction[CreateClusterParams] { (request, body) =>
    Authenticated(request) { profile =>
      Try(serviceManager.createCluster(
        body.name, body.size, serviceManager.services(profile.toClusterUser))) match {
        case Failure(ex) => throw ex
        case Success(clusterId: ClusterId) => {
          Logger.info(s"Provisioning new cluster $clusterId")
          DB.withTransaction { implicit c =>
            CosmosProfileDao.assignCluster(clusterId, profile.id)
          }
          val reference: ClusterReference = ClusterReference(clusterId)(request)
          Created(Json.toJson(reference)).withHeaders(LOCATION -> reference.href)
        }
      }
    }
  }
}
