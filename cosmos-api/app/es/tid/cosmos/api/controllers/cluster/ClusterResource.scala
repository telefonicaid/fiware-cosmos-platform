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

package es.tid.cosmos.api.controllers.cluster

import javax.ws.rs.PathParam
import scala.util.{Failure, Success, Try}

import com.wordnik.swagger.annotations._
import play.api.Play.current
import play.api.db.DB
import play.api.libs.json._
import play.api.mvc.{Result, RequestHeader, Action}

import es.tid.cosmos.api.controllers.common.{AuthController, Message}
import es.tid.cosmos.api.controllers.pages.CosmosProfile
import es.tid.cosmos.api.profile.CosmosProfileDao
import es.tid.cosmos.servicemanager.{ClusterDescription, ClusterId, ServiceManager}

/**
 * Resource that represents a single cluster.
 */
@Api(value = "/cosmos/cluster", listingPath = "/doc/cosmos/cluster",
  description = "Represents an existing or decommissioned cluster")
class ClusterResource(serviceManager: ServiceManager) extends AuthController {
  @ApiOperation(value = "Get cluster machines", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.cluster.ClusterDetails")
  @ApiErrors(Array(
    new ApiError(code = 404, reason = "When cluster ID is unknown")
  ))
  def listDetails(
      @ApiParam(value = "Cluster identifier", required = true,
        defaultValue = "00000000-0000-0000-0000-000000000000")
      @PathParam("id")
      id: String) = Action { implicit request =>
    Authenticated(request) { profile =>
      OwnedCluster(profile, ClusterId(id)) { cluster =>
        Ok(Json.toJson(ClusterDetails(cluster)))
      }
    }
  }

  @ApiOperation(value = "Terminate cluster", httpMethod = "POST", notes = "No body is required",
    responseClass = "es.tid.cosmos.api.controllers.common.Message")
  @ApiErrors(Array(
    new ApiError(code = 500, reason = "When cluster does not exists or cannot be terminated")
  ))
  def terminate(
       @ApiParam(value = "Cluster identifier", required = true,
         defaultValue = "00000000-0000-0000-0000-000000000000")
       @PathParam("id")
       id: String) = Action { request =>
    Authenticated(request) { profile =>
      OwnedCluster(profile, ClusterId(id)) { cluster =>
        Try(serviceManager.terminateCluster(cluster.id)) match {
          case Success(_) => Ok(Json.toJson(Message("Terminating cluster")))
          case Failure(ex) => throw ex
        }
      }
    }
  }

  private def OwnedCluster(
      profile: CosmosProfile, clusterId: ClusterId)(f: ClusterDescription => Result) = {
    val owned = isOwnCluster(profile.id, clusterId)
    val maybeDescription = serviceManager.describeCluster(clusterId)
    (owned, maybeDescription) match {
      case (true, None) => throw new IllegalStateException(
        s"Cluster '$clusterId' is on app database but not found in Service Manager")
      case (false, None) => notFound(clusterId)
      case (false, Some(_)) => unauthorizedAccessTo(clusterId)
      case (true, Some(description)) => f(description)
    }
  }

  private def isOwnCluster(cosmosId: Long, cluster: ClusterId): Boolean =
    DB.withConnection { implicit c =>
      val ownedClusters = CosmosProfileDao.clustersOf(cosmosId)(DB.getConnection())
      ownedClusters.contains(cluster)
    }

  private def unauthorizedAccessTo(cluster: ClusterId) =
    Unauthorized(Json.toJson(Message(s"Cannot access cluster '$cluster'")))

  private def notFound(cluster: ClusterId) =
    NotFound(Json.toJson(Message(s"No cluster '$cluster' exists")))
}

object ClusterResource {
  def clusterUrl(id: ClusterId)(implicit request: RequestHeader): String =
    routes.ClusterResource.listDetails(id.toString).absoluteURL(secure = false)
}