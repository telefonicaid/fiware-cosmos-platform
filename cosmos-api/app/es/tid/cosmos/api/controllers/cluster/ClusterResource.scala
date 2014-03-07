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
import scala.util.{Try, Failure, Success}
import scalaz._

import com.wordnik.swagger.annotations._
import play.Logger
import play.api.libs.json._
import play.api.mvc._

import es.tid.cosmos.api.auth.request.RequestAuthentication
import es.tid.cosmos.api.controllers.admin.MaintenanceStatus
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.common.auth.ApiAuthController
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.task.TaskDao
import es.tid.cosmos.servicemanager.{ClusterExecutableValidation, ClusterUser, ServiceManager}
import es.tid.cosmos.servicemanager.clusters.{ClusterId, ClusterDescription}

/** Resource that represents a single cluster. */
@Api(value = "/cosmos/v1/cluster", listingPath = "/doc/cosmos/v1/cluster",
  description = "Represents an existing or decommissioned cluster")
class ClusterResource(
    override val auth: RequestAuthentication,
    serviceManager: ServiceManager,
    override val taskDao: TaskDao,
    dao: CosmosDao,
    override val maintenanceStatus: MaintenanceStatus) extends Controller with ApiAuthController
  with JsonController with MaintenanceAwareController with TaskController {

  import ClusterResource._

  private lazy val quotaContext =
    new QuotaContextFactory(new CosmosMachineUsageDao(dao, serviceManager))

  /** List user clusters. */
  @ApiOperation(value = "List clusters", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.cluster.ClusterList")
  def list = Action { implicit request =>

    def listClustersWithOwnConnection(profile: CosmosProfile) = dao.store.withConnection { c =>
      listClusters(profile)(c)
    }

    for {
      profile <- requireAuthenticatedApiRequest(request)
    } yield Ok(Json.toJson(
      ClusterList(listClustersWithOwnConnection(profile).map(_.withAbsoluteUri(request)))
    ))
  }

  /** Start a new cluster provisioning. */
  @ApiOperation(value = "Create a new cluster", httpMethod = "POST")
  @ApiErrors(Array(
    new ApiError(code = 400, reason = "Invalid JSON payload"),
    new ApiError(code = 403, reason = "Quota exceeded"),
    new ApiError(code = 503, reason = "Service is under maintenance")
  ))
  @ApiParamsImplicit(Array(
    new ApiParamImplicit(paramType = "body",
      dataType = "es.tid.cosmos.api.controllers.cluster.CreateClusterParams")
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
      case Success(clusterId: ClusterId) =>
        Logger.info(s"Provisioning new cluster $clusterId")
        val assignment = ClusterAssignment(clusterId, profile.id, new Date())
        dao.store.withTransaction { implicit c => dao.cluster.register(assignment) }
        val clusterDescription = serviceManager.describeCluster(clusterId).get
        val reference = ClusterReference(clusterDescription, assignment).withAbsoluteUri(request)
        Created(Json.toJson(reference)).withHeaders(LOCATION -> reference.href)
    }
  }

  private def executableWithinQuota(profile: CosmosProfile, size: Int): ClusterExecutableValidation =
    (id: ClusterId) => () => quotaContext(Some(id)).withinQuota(profile, size)

  private def requireWithinQuota(profile: CosmosProfile, size: Int): ActionValidation[Int] =
    quotaContext().withinQuota(profile, size).leftMap(errors =>
      Forbidden(Json.toJson(Message(errors.list.mkString(" "))))
    )

  private def listClusters(profile: CosmosProfile)(implicit c: dao.Conn) = {
    val assignedClusters =  Set(dao.cluster.ownedBy(profile.id): _*)
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
        defaultValue = "00000000000000000000000000000000")
      @PathParam("id")
      id: String) = Action { implicit request =>
    for {
      profile <- requireAuthenticatedApiRequest(request)
      cluster <- requireSshAccessToCluster(profile, ClusterId(id))
    } yield Ok(Json.toJson(ClusterDetails(cluster)))
  }

  @ApiOperation(value = "Add users to an existing cluster", httpMethod = "POST")
  @ApiParamsImplicit(Array(
    new ApiParamImplicit(paramType = "body",
      dataType = "es.tid.cosmos.api.controllers.cluster.ManageUserParams")
  ))
  def addUser(
      @ApiParam(value = "Cluster identifier", required = true,
        defaultValue = "00000000000000000000000000000000")
      @PathParam("id")
      id: String) = Action(parse.tolerantJson) { request =>
    val clusterId = ClusterId(id)
    for {
      user <- requireUserManagementConditions(request, clusterId)
      _ <- requireProfileIsNotUserOf(user, clusterId)
      _ <- requireNoActiveTask(resource = request.uri, metadata = user.handle)
      clusterUsers <- requireClusterUsersAreAvailable(clusterId)
    } yield {
      val addUser_> = serviceManager.addUser(clusterId, ClusterUser.enabled(
        username = user.handle,
        publicKey = user.keys.head.signature,
        isSudoer = user.capabilities.hasCapability(Capability.IsSudoer)
      ))
      val task = taskDao.registerTask().linkToFuture(
        future = addUser_>,
        errorMessage = s"Failed to add user ${user.handle} to cluster")
      task.usersWithAccess = user.handle +: clusterUsers.map(_.username)
      task.metadata = user.handle
      task.resource = request.uri
      Ok(Json.toJson(Message(s"Adding user ${user.handle} to cluster $clusterId " +
        "(this will take a while, please be patient)")))
    }
  }

  @ApiOperation(value = "Remove a user from an existing cluster", httpMethod = "POST")
  @ApiParamsImplicit(Array(
    new ApiParamImplicit(paramType = "body",
      dataType = "es.tid.cosmos.api.controllers.cluster.ManageUserParams")
  ))
  def removeUser(
      @ApiParam(value = "Cluster identifier", required = true,
        defaultValue = "00000000000000000000000000000000")
      @PathParam("id")
      id: String) = Action(parse.tolerantJson) { request =>
    val clusterId = ClusterId(id)
    for {
      user <- requireUserManagementConditions(request, clusterId)
      _ <- requireProfileIsUserOf(user, clusterId)
      _ <- requireNotOwnedCluster(user, clusterId)
      _ <- requireClusterUsersAreAvailable(clusterId)
    } yield {
      serviceManager.disableUser(clusterId, user.handle)
      Ok(Json.toJson(Message(s"Removing user ${user.handle} from cluster $clusterId " +
        "(this will take a while, please be patient)")))
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
         defaultValue = "00000000000000000000000000000000")
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

  private def requireSshAccessToCluster(
      profile: CosmosProfile, clusterId: ClusterId): ActionValidation[ClusterDescription] = {
    import Scalaz._
    requireThatOwner(profile, clusterId,
      ifOwner = desc => desc.success,
      ifNotOwner = desc => (for {
        users <- desc.users
        if users.exists(user => user.sshEnabled && (user.username == profile.handle))
      } yield desc).toSuccess(unauthorizedAccessTo(clusterId)))
  }

  private def requireOwnedCluster(
      profile: CosmosProfile, clusterId: ClusterId): ActionValidation[ClusterDescription] = {
    import Scalaz._
    requireThatOwner(profile, clusterId,
      ifOwner = desc => desc.success,
      ifNotOwner = _ => unauthorizedAccessTo(clusterId).failure
    )
  }

  private def requireNotOwnedCluster(
      profile: CosmosProfile,
      clusterId: ClusterId): ActionValidation[ClusterDescription] = {
    import Scalaz._
    requireThatOwner(profile, clusterId,
      ifOwner = _ => userIsOwnerOf(profile.handle, clusterId).failure,
      ifNotOwner = desc => desc.success
    )
  }

  private def requireThatOwner(
      profile: CosmosProfile,
      clusterId: ClusterId,
      ifOwner: ClusterDescription => ActionValidation[ClusterDescription],
      ifNotOwner: ClusterDescription => ActionValidation[ClusterDescription]):
      ActionValidation[ClusterDescription] = {
    import Scalaz._
    clusterOwnership(profile, clusterId) match {
      case (true, None) => throw new IllegalStateException(
        s"Cluster '$clusterId' is on app database but not found in Service Manager")
      case (false, None) => notFound(clusterId).failure
      case (false, Some(description)) => ifNotOwner(description)
      case (true, Some(description)) => ifOwner(description)
    }
  }

  private def clusterOwnership(profile: CosmosProfile, clusterId: ClusterId) = {
    val owned = isOwnerOfCluster(profile.id, clusterId)
    val maybeDescription = serviceManager.describeCluster(clusterId)
    (owned, maybeDescription)
  }

  private def requireProfileExists(handle: String): ActionValidation[CosmosProfile] = {
    import Scalaz._
    dao.store.withConnection { implicit c =>
      dao.profile.lookupByHandle(handle) match {
        case Some(profile: CosmosProfile) => profile.success
        case None => profileNotFound(handle).failure
      }
    }
  }

  private def requireProfileIsUserOf(
      profile: CosmosProfile,
      clusterId: ClusterId): ActionValidation[Unit] = {
    import Scalaz._
    requireClusterUsersAreAvailable(clusterId).flatMap { users =>
      if (userIsMemberOfCluster(profile, users)) ().success
      else notUserOf(profile.handle, clusterId).failure
    }
  }

  private def userIsMemberOfCluster(profile: CosmosProfile, users: Seq[ClusterUser]) =
    users.exists(usr => usr.username.equals(profile.handle) && usr.isEnabled)

  private def requireProfileIsNotUserOf(
      profile: CosmosProfile,
      clusterId: ClusterId): ActionValidation[Unit] = {
    import Scalaz._
    requireProfileIsUserOf(profile, clusterId).fold(
      fail = _ => ().success,
      succ = _ => alreadyUserOf(profile.handle, clusterId).failure
    )
  }

  private def requireClusterUsersAreAvailable(
      clusterId: ClusterId): ActionValidation[Seq[ClusterUser]] = {
    import Scalaz._
    serviceManager.listUsers(clusterId) match {
      case Some(users) => users.success
      case None => usersUnavailable(clusterId).failure
    }
  }

  private def requireUserManagementConditions(
      request: Request[JsValue], clusterId: ClusterId): ActionValidation[CosmosProfile] = for {
    requester <- requireAuthenticatedApiRequest(request)
    _ <- requireNotUnderMaintenanceToNonOperators(requester)
    body <- validJsonBody[ManageUserParams](request)
    _ <- requireOwnedCluster(requester, clusterId)
    user <- requireProfileExists(body.user)
  } yield user

  private def isOwnerOfCluster(cosmosId: Long, cluster: ClusterId): Boolean =
    dao.store.withConnection { implicit c =>
      dao.cluster.ownedBy(cosmosId).exists(_.clusterId == cluster)
    }
}

object ClusterResource extends Results {
  def clusterUrl(id: ClusterId)(implicit request: RequestHeader): String =
    AbsoluteUrl(routes.ClusterResource.listDetails(id.toString))

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

  private def usersUnavailable(clusterId: ClusterId) =
    NotFound(Json.toJson(ErrorMessage(s"User service is unavailable for cluster '$clusterId'")))
}
