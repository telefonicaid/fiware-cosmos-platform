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
import scalaz._

import com.wordnik.swagger.annotations._
import play.Logger
import play.api.libs.json._
import play.api.mvc.{Request, SimpleResult, RequestHeader, Action}

import es.tid.cosmos.api.controllers.admin.MaintenanceStatus
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile._
import es.tid.cosmos.servicemanager.{ClusterExecutableValidation, ClusterUser, ServiceManager}
import es.tid.cosmos.servicemanager.clusters.{ClusterId, ClusterDescription}

/**
 * Resource that represents a single cluster.
 */
@Api(value = "/cosmos/v1/cluster", listingPath = "/doc/cosmos/v1/cluster",
  description = "Represents an existing or decommissioned cluster")
class ClusterResource(
    serviceManager: ServiceManager,
    override val dao: CosmosProfileDao,
    override val maintenanceStatus: MaintenanceStatus)
  extends ApiAuthController with JsonController with MaintenanceAwareController {

  /**
   * List user clusters.
   */
  @ApiOperation(value = "List clusters", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.clusters.ClusterList")
  def list = Action { implicit request =>
    for {
      profile <- requireAuthenticatedApiRequest(request)
    } yield Ok(Json.toJson(ClusterList(listClusters(profile).map(_.withAbsoluteUri(request)))))
  }

  /**
   * Start a new cluster provisioning.
   */
  @ApiOperation(value = "Create a new cluster", httpMethod = "POST")
  @ApiErrors(Array(
    new ApiError(code = 400, reason = "Invalid JSON payload"),
    new ApiError(code = 403, reason = "Quota exceeded"),
    new ApiError(code = 503, reason = "Service is under maintenance")
  ))
  @ApiParamsImplicit(Array(
    new ApiParamImplicit(paramType = "body",
      dataType = "es.tid.cosmos.api.controllers.clusters.CreateClusterParams")
  ))
  def createCluster = Action(parse.tolerantJson) { request =>
    for {
      profile <- requireAuthenticatedApiRequest(request)
      _ <- requireNotUnderMaintenanceToNonOperators(profile)
      body <- validJsonBody[CreateClusterParams](request)
      _ <- requireWithinQuota(profile, body.size)
    } yield create(request, body, profile)
  }

  private def create(
      request: Request[JsValue],
      body: CreateClusterParams,
      profile: CosmosProfile): SimpleResult = {

    val services = serviceManager.services.filter(
      service => body.optionalServices.contains(service.name))
    Try(serviceManager.createCluster(
      name = body.name,
      clusterSize = body.size,
      serviceDescriptions = services,
      users = Seq(ClusterUser(
        userName = profile.handle,
        publicKey = profile.keys.head.signature,
        isSudoer = profile.capabilities.hasCapability(Capability.IsSudoer)
      )),
      preConditions = executableWithinQuota(profile, body.size)
    )) match {
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
  }

  private def withinQuota(
      profile: CosmosProfile,
      size: Int,
      requestedClusterId: Option[ClusterId] = None): ValidationNel[String, Int] =

    dao.withConnection { implicit c =>
      new ProfileQuotas(
        machinePoolSize = serviceManager.clusterNodePoolCount,
        groups = dao.getGroups,
        lookupByGroup = dao.lookupByGroup,
        listClusters = listClusters
      ).withinQuota(profile, size, requestedClusterId)
    }

  private def executableWithinQuota(profile: CosmosProfile, size: Int): ClusterExecutableValidation =
    (id: ClusterId) => () => withinQuota(profile, size, Some(id))

  private def requireWithinQuota(profile: CosmosProfile, size: Int): ActionValidation[Int] =
    withinQuota(profile, size).leftMap(errors =>
      Forbidden(Json.toJson(Message(errors.list.mkString(" "))))
    )

  private def listClusters(profile: CosmosProfile) = {
    val assignedClusters = dao.withConnection { implicit c =>
      Set(dao.clustersOf(profile.id): _*)
    }
    (for {
      assignment <- assignedClusters.toList
      description <- serviceManager.describeCluster(assignment.clusterId).toList
    } yield ClusterReference(description, assignment)).sorted(ClustersDisplayOrder)
  }

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
    for {
      profile <- requireAuthenticatedApiRequest(request)
      cluster <- requireOwnedCluster(profile.id, ClusterId(id))
    } yield Ok(Json.toJson(ClusterDetails(cluster)))
  }

  @ApiOperation(value = "Terminate cluster", httpMethod = "POST", notes = "No body is required",
    responseClass = "es.tid.cosmos.api.controllers.common.Message")
  @ApiErrors(Array(
    new ApiError(code = 500, reason = "Internal server error"),
    new ApiError(code = 404, reason = "Cluster cannot be found"),
    new ApiError(code = 409, reason = "Cluster cannot be terminated"),
    new ApiError(code = 503, reason = "Service is under maintenance")
  ))
  def terminate(
       @ApiParam(value = "Cluster identifier", required = true,
         defaultValue = "00000000-0000-0000-0000-000000000000")
       @PathParam("id")
       id: String) = Action { request =>
    for {
      profile <- requireAuthenticatedApiRequest(request)
      _ <- requireNotUnderMaintenanceToNonOperators(profile)
      cluster <- requireOwnedCluster(profile.id, ClusterId(id))
    } yield Try(serviceManager.terminateCluster(cluster.id)) match {
      case Success(_) => Ok(Json.toJson(Message("Terminating cluster")))
      case Failure(ex: IllegalArgumentException) => Conflict(Json.toJson(
        ErrorMessage(ex.getMessage, ex)))
      case Failure(ex) => throw ex
    }
  }

  private def requireOwnedCluster(
      profileId: ProfileId, clusterId: ClusterId): ActionValidation[ClusterDescription] = {
    import Scalaz._
    val owned = isOwnCluster(profileId, clusterId)
    val maybeDescription = serviceManager.describeCluster(clusterId)
    (owned, maybeDescription) match {
      case (true, None) => throw new IllegalStateException(
        s"Cluster '$clusterId' is on app database but not found in Service Manager")
      case (false, None) => notFound(clusterId).failure
      case (false, Some(_)) => unauthorizedAccessTo(clusterId).failure
      case (true, Some(description)) => description.success
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
