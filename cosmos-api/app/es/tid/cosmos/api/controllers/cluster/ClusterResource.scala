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

    val services = serviceManager.optionalServices.filter(
      service => body.optionalServices.contains(service.name))
    Try(serviceManager.createCluster(
      name = body.name,
      clusterSize = body.size,
      serviceDescriptions = services,
      users = Seq(ClusterUser(
        username = profile.handle,
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
      cluster <- requireOwnedCluster(profile, ClusterId(id))
    } yield Ok(Json.toJson(ClusterDetails(cluster)))
  }

  @ApiOperation(value = "Add users to an existing cluster", httpMethod = "POST")
  @ApiParamsImplicit(Array(
    new ApiParamImplicit(paramType = "body",
      dataType = "es.tid.cosmos.api.controllers.clusters.ManageUserParams")
  ))
  def addUser(
      @ApiParam(value = "Cluster identifier", required = true,
        defaultValue = "00000000-0000-0000-0000-000000000000")
      @PathParam("id")
      id: String) = Action(parse.tolerantJson) { request =>
    val clusterId = ClusterId(id)
    for {
      requester <- requireAuthenticatedApiRequest(request)
      _ <- requireNotUnderMaintenanceToNonOperators(requester)
      body <- validJsonBody[ManageUserParams](request)
      _ <- requireOwnedCluster(requester, clusterId)
      user <- requireProfileExists(body.user)
      _ <- requireProfileIsNotUserOf(user, clusterId)
    } yield addUserToCluster(user, clusterId)
  }

  private def addUserToCluster(user: CosmosProfile, clusterId: ClusterId): SimpleResult = {
    Try({
      val currentUsers = serviceManager.listUsers(clusterId).getOrElse(
        throw new IllegalArgumentException(
          s"cannot add user to cluster: no such cluster with id $clusterId")
      )
      val newUsers = currentUsers.filterNot(_.username.equals(user.handle)) :+ ClusterUser(
        username = user.handle,
        publicKey = user.keys.head.signature,
        isSudoer = user.capabilities.hasCapability(Capability.IsSudoer),
        sshEnabled = true,
        hdfsEnabled = true
      )
      serviceManager.setUsers(clusterId, newUsers)
    }) match {
      case Success(_) => Ok(Json.toJson(
        Message(s"Adding user ${user.handle} to cluster $clusterId " +
          s"(this will take a while, please be patient)")))
      case Failure(ex: IllegalArgumentException) => Conflict(Json.toJson(
        ErrorMessage(ex.getMessage, ex)))
      case Failure(ex) => throw ex
    }
  }

  @ApiOperation(value = "Remove a user from an existing cluster", httpMethod = "POST")
  @ApiParamsImplicit(Array(
    new ApiParamImplicit(paramType = "body",
      dataType = "es.tid.cosmos.api.controllers.clusters.ManageUserParams")
  ))
  def removeUser(
      @ApiParam(value = "Cluster identifier", required = true,
        defaultValue = "00000000-0000-0000-0000-000000000000")
      @PathParam("id")
      id: String) = Action(parse.tolerantJson) { request =>
    val clusterId = ClusterId(id)
    for {
      requester <- requireAuthenticatedApiRequest(request)
      _ <- requireNotUnderMaintenanceToNonOperators(requester)
      body <- validJsonBody[ManageUserParams](request)
      _ <- requireOwnedCluster(requester, clusterId)
      user <- requireProfileExists(body.user)
      _ <- requireProfileIsUserOf(user, clusterId)
      _ <- requireNotOwnedCluster(user, clusterId)
    } yield removeUserFromCluster(user, clusterId)
  }

  private def removeUserFromCluster(user: CosmosProfile, cluster: ClusterId): SimpleResult = {
    Try({
      val currentUsers = serviceManager.listUsers(cluster).getOrElse(
        throw new IllegalArgumentException(
          s"cannot remove user from cluster: no such cluster with id $cluster")
      )
      val newUsers = currentUsers.filterNot(_.username.equals(user.handle)) :+ ClusterUser(
        username = user.handle,
        publicKey = user.keys.head.signature,
        isSudoer = false,
        sshEnabled = false,
        hdfsEnabled = false
      )
      serviceManager.setUsers(cluster, newUsers)
    }) match {
      case Success(_) => Ok(Json.toJson(
        Message(s"Removing user ${user.handle} from cluster $cluster " +
          s"(this will take a while, please be patient)")))
      case Failure(ex: IllegalArgumentException) =>
        Conflict(Json.toJson(ErrorMessage(ex.getMessage, ex)))
      case Failure(ex) => throw ex
    }
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
      cluster <- requireOwnedCluster(profile, ClusterId(id))
    } yield Try(serviceManager.terminateCluster(cluster.id)) match {
      case Success(_) => Ok(Json.toJson(Message("Terminating cluster")))
      case Failure(ex: IllegalArgumentException) => Conflict(Json.toJson(
        ErrorMessage(ex.getMessage, ex)))
      case Failure(ex) => throw ex
    }
  }

  private def requireOwnedCluster(
      profile: CosmosProfile, clusterId: ClusterId): ActionValidation[ClusterDescription] = {
    import Scalaz._
    clusterOwnership(profile, clusterId) match {
      case (true, None) => throw new IllegalStateException(
        s"Cluster '$clusterId' is on app database but not found in Service Manager")
      case (false, None) => notFound(clusterId).failure
      case (false, Some(_)) => unauthorizedAccessTo(clusterId).failure
      case (true, Some(description)) => description.success
    }
  }

  private def requireNotOwnedCluster(
      profile: CosmosProfile,
      clusterId: ClusterId): ActionValidation[ClusterDescription] = {
    import Scalaz._
    clusterOwnership(profile, clusterId) match {
      case (true, None) => throw new IllegalStateException(
        s"Cluster '$clusterId' is on app database but not found in Service Manager")
      case (false, None) => notFound(clusterId).failure
      case (false, Some(description)) => description.success
      case (true, Some(_)) => userIsOwnerOf(profile.handle, clusterId).failure
    }
  }

  private def clusterOwnership(profile: CosmosProfile, clusterId: ClusterId) = {
    val owned = isOwnCluster(profile.id, clusterId)
    val maybeDescription = serviceManager.describeCluster(clusterId)
    (owned, maybeDescription)
  }

  private def requireProfileExists(handle: String): ActionValidation[CosmosProfile] = {
    import Scalaz._
    dao.withConnection { implicit c =>
      dao.lookupByHandle(handle) match {
        case Some(profile: CosmosProfile) => profile.success
        case None => profileNotFound(handle).failure
      }
    }
  }

  private def requireProfileIsUserOf(
      profile: CosmosProfile,
      clusterId: ClusterId): ActionValidation[Unit] = {
    import Scalaz._
    if (isUserOf(profile, clusterId)) { ().success }
    else { notUserOf(profile.handle, clusterId).failure }
  }

  private def requireProfileIsNotUserOf(
      profile: CosmosProfile,
      clusterId: ClusterId): ActionValidation[Unit] = {
    import Scalaz._
    if (!isUserOf(profile, clusterId)) { ().success }
    else { alreadyUserOf(profile.handle, clusterId).failure }
  }

  private def isUserOf(profile: CosmosProfile, clusterId: ClusterId): Boolean =
    serviceManager.listUsers(clusterId) match {
      case Some(users) => {
        val user = users.find(_.username.equals(profile.handle))
        user.isDefined && user.get.sshEnabled
      }
      case None => throw new IllegalStateException(
        s"No cluster users found in Service Manager for cluster '$clusterId'")
    }

  private def isOwnCluster(cosmosId: Long, cluster: ClusterId): Boolean =
    dao.withConnection { implicit c =>
      dao.clustersOf(cosmosId).exists(_.clusterId == cluster)
    }

  private def unauthorizedAccessTo(cluster: ClusterId) =
    Unauthorized(Json.toJson(ErrorMessage(s"Cannot access cluster '$cluster'")))

  private def userIsOwnerOf(handle: String, cluster: ClusterId) =
    BadRequest(Json.toJson(ErrorMessage(s"User $handle is the owner of cluster '$cluster'")))

  private def notUserOf(handle: String, cluster: ClusterId) =
    BadRequest(Json.toJson(ErrorMessage(s"User $handle is not a user of cluster '$cluster'")))

  private def alreadyUserOf(handle: String, cluster: ClusterId) =
    BadRequest(Json.toJson(ErrorMessage(s"User $handle is already a user of cluster '$cluster'")))

  private def notFound(cluster: ClusterId) =
    NotFound(Json.toJson(ErrorMessage(s"No cluster '$cluster' exists")))

  private def profileNotFound(handle: String) =
    NotFound(Json.toJson(ErrorMessage(s"No user was found with handle '$handle'")))
}

object ClusterResource {
  def clusterUrl(id: ClusterId)(implicit request: RequestHeader): String =
    AbsoluteUrl(routes.ClusterResource.listDetails(id.toString))
}
