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

import java.util.Date
import javax.ws.rs.PathParam
import scala.util.{Failure, Success, Try}

import com.wordnik.swagger.annotations._
import play.Logger
import play.api.libs.json._
import play.api.mvc.{Result, RequestHeader, Action}

import es.tid.cosmos.api.controllers._
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile.{CosmosProfile, ClusterAssignment, CosmosProfileDao}
import es.tid.cosmos.servicemanager.{ClusterDescription, ClusterId, ServiceManager}

/**
 * Resource that represents a single cluster.
 */
@Api(value = "/cosmos/v1/cluster", listingPath = "/doc/cosmos/v1/cluster",
  description = "Represents an existing or decommissioned cluster")
class ClusterResource(serviceManager: ServiceManager, override val dao: CosmosProfileDao)
  extends ApiAuthController with JsonController {

  /**
   * List user clusters.
   */
  @ApiOperation(value = "List clusters", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.clusters.ClusterList")
  def list = Action { implicit request =>
    withApiAuth(request) { profile =>
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
    withApiAuth(request) { profile =>
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
    withApiAuth(request) { profile =>
      OwnedCluster(profile, ClusterId(id)) { cluster =>
        Ok(Json.toJson(ClusterDetails(cluster)))
      }
    }
  }

  @ApiOperation(value = "Terminate cluster", httpMethod = "POST", notes = "No body is required",
    responseClass = "es.tid.cosmos.api.controllers.common.Message")
  @ApiErrors(Array(
    new ApiError(code = 500, reason = "Internal server error"),
    new ApiError(code = 404, reason = "Cluster cannot be found"),
    new ApiError(code = 409, reason = "Cluster cannot be terminated")
  ))
  def terminate(
       @ApiParam(value = "Cluster identifier", required = true,
         defaultValue = "00000000-0000-0000-0000-000000000000")
       @PathParam("id")
       id: String) = Action { request =>
    withApiAuth(request) { profile =>
      OwnedCluster(profile, ClusterId(id)) { cluster =>
        Try(serviceManager.terminateCluster(cluster.id)) match {
          case Success(_) => Ok(Json.toJson(Message("Terminating cluster")))
          case Failure(ex: IllegalArgumentException) => Conflict(Json.toJson(
            ErrorMessage(ex.getMessage, ex)))
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
    dao.withConnection { implicit c =>
      dao.clustersOf(cosmosId).exists(_.clusterId == cluster)
    }

  private def unauthorizedAccessTo(cluster: ClusterId) =
    Unauthorized(Json.toJson(ErrorMessage(s"Cannot access cluster '$cluster'")))

  private def notFound(cluster: ClusterId) =
    NotFound(Json.toJson(ErrorMessage(s"No cluster '$cluster' exists")))
}

object ClusterResource {
  def clusterUrl(id: ClusterId)(implicit request: RequestHeader): String =
    AbsoluteUrl(routes.ClusterResource.listDetails(id.toString))
}
