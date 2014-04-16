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

package es.tid.cosmos.api.controllers.cluster

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
import es.tid.cosmos.api.profile.{ClusterSecret, Capability, CosmosProfile, QuotaContextFactory}
import es.tid.cosmos.api.profile.dao.{ClusterDataStore, ProfileDataStore}
import es.tid.cosmos.api.quota.{Group, NoGroup}
import es.tid.cosmos.api.task.TaskDao
import es.tid.cosmos.api.usage.MachineUsage
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.clusters.{ClusterId, ClusterDescription}
import es.tid.cosmos.servicemanager.services.{InfinityDriver, CosmosUserService, Hdfs}
import es.tid.cosmos.servicemanager.services.Hdfs.HdfsParameters
import es.tid.cosmos.servicemanager.services.InfinityDriver.InfinityDriverParameters

/** Resource that represents a single cluster. */
@Api(value = "/cosmos/v1/cluster", listingPath = "/doc/cosmos/v1/cluster",
  description = "Represents an existing or decommissioned cluster")
class ClusterResource(
    override val auth: RequestAuthentication,
    serviceManager: ServiceManager,
    machineUsage: MachineUsage,
    override val taskDao: TaskDao,
    store: ProfileDataStore with ClusterDataStore,
    override val maintenanceStatus: MaintenanceStatus) extends Controller with ApiAuthController
  with JsonController with MaintenanceAwareController with TaskController {

  import ClusterResource._
  import Scalaz._

  private lazy val quotaContext = new QuotaContextFactory(machineUsage)

  /** List user clusters. */
  @ApiOperation(value = "List clusters", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.cluster.ClusterList")
  def list = Action { implicit request =>
    for {
      profile <- requireAuthenticatedApiRequest(request)
    } yield {
      val clusters = store.withConnection { implicit c =>
        listClusters(profile)
      }.map(_.withAbsoluteUri(request))
      Ok(Json.toJson(ClusterList(clusters)))
    }
  }

  /** Start a new cluster provisioning. */
  @ApiOperation(value = "Create a new cluster", httpMethod = "POST")
  @ApiErrors(Array(
    new ApiError(code = 400, reason = "Invalid JSON payload"),
    new ApiError(
      code = 400, reason = "Shared cluster requested by user that doesn't belong to any group"),
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
      _ <- requireGroupIfShared(profile.group, body.shared)
    } yield create(request, body, profile)
  }

  private def create(
      request: Request[JsValue],
      clusterParameters: CreateClusterParams,
      profile: CosmosProfile): SimpleResult = {

    val users = usersForCluster(clusterParameters, profile)
    val clusterSecret = ClusterSecret.random()
    val clusterId = serviceManager.createCluster(
      name = clusterParameters.name,
      clusterSize = clusterParameters.size,
      services = servicesToInstall(clusterParameters, users, clusterSecret),
      users = users,
      preConditions = executableWithinQuota(profile, clusterParameters.size)
    )
    Logger.info(s"Provisioning new cluster $clusterId")
    val cluster = store.withTransaction { implicit c =>
      store.cluster.register(clusterId, profile.id, clusterSecret, clusterParameters.shared)
    }
    val clusterDescription = serviceManager.describeCluster(clusterId).get
    val reference = ClusterReference(clusterDescription, cluster).withAbsoluteUri(request)
    Created(Json.toJson(reference)).withHeaders(LOCATION -> reference.href)
  }

  private def servicesToInstall(
      clusterParameters: CreateClusterParams,
      users: Seq[ClusterUser],
      clusterSecret: ClusterSecret): Set[AnyServiceInstance] = {
    val cosmosUsers = CosmosUserService.instance(users)
    val hdfs = Hdfs.defaultInstance.get
    val infinityDriver = InfinityDriver.instance(InfinityDriverParameters(clusterSecret.underlying))
    val optionalServices = serviceManager.optionalServices.filter(
      service => clusterParameters.optionalServices.contains(service.name)
    ).map(_.defaultInstance.get)
    Set(cosmosUsers, hdfs, infinityDriver) ++ optionalServices
  }

  private def usersForCluster(
      clusterParams: CreateClusterParams, profile: CosmosProfile): Seq[ClusterUser] =
    if (!clusterParams.shared) {
      Seq(toClusterUser(profile))
    } else {
      assert(profile.group != NoGroup)
      store.withTransaction { implicit c =>
        store.profile.lookupByGroup(profile.group)
      }.toSeq.map(toClusterUser)
    }

  private def toClusterUser(profile: CosmosProfile) = ClusterUser(
    username = profile.handle,
    publicKey = profile.keys.head.signature,
    isSudoer = profile.capabilities.hasCapability(Capability.IsSudoer)
  )

  private def executableWithinQuota(profile: CosmosProfile, size: Int): ClusterExecutableValidation =
    (id: ClusterId) => () => quotaContext(Some(id)).withinQuota(profile, size)

  private def requireWithinQuota(profile: CosmosProfile, size: Int): ActionValidation[Int] =
    quotaContext().withinQuota(profile, size).leftMap(errors =>
      Forbidden(Json.toJson(Message(errors.list.mkString(" "))))
    )

  private def listClusters(profile: CosmosProfile)
                          (implicit c: store.Conn): Seq[ClusterReference] = {
    val assignedClusters =  Set(store.cluster.ownedBy(profile.id): _*)
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
    } yield Ok(Json.toJson(ClusterDetails(
      cluster,
      store.withTransaction(implicit c => store.cluster.get(cluster.id)))))
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
    requireThatOwner(profile, clusterId,
      ifOwner = desc => desc.success,
      ifNotOwner = desc => (for {
        users <- desc.users
        if users.exists(user => user.sshEnabled && (user.username == profile.handle))
      } yield desc).toSuccess(unauthorizedAccessTo(clusterId)))
  }

  private def requireOwnedCluster(
      profile: CosmosProfile, clusterId: ClusterId): ActionValidation[ClusterDescription] =
    requireThatOwner(profile, clusterId,
      ifOwner = desc => desc.success,
      ifNotOwner = _ => unauthorizedAccessTo(clusterId).failure
    )

  private def requireThatOwner(
      profile: CosmosProfile,
      clusterId: ClusterId,
      ifOwner: ClusterDescription => ActionValidation[ClusterDescription],
      ifNotOwner: ClusterDescription => ActionValidation[ClusterDescription]):
      ActionValidation[ClusterDescription] = clusterOwnership(profile, clusterId) match {
    case (true, None) => throw new IllegalStateException(
      s"Cluster '$clusterId' is on app database but not found in Service Manager")
    case (false, None) => notFound(clusterId).failure
    case (false, Some(description)) => ifNotOwner(description)
    case (true, Some(description)) => ifOwner(description)
  }

  private def clusterOwnership(profile: CosmosProfile, clusterId: ClusterId) = {
    val owned = isOwnerOfCluster(profile.id, clusterId)
    val maybeDescription = serviceManager.describeCluster(clusterId)
    (owned, maybeDescription)
  }

  private def requireGroupIfShared(group: Group, shared: Boolean): ActionValidation[Unit] =
    if (shared && group == NoGroup) sharedClusterNoGroup.failure
    else ().success

  private def isOwnerOfCluster(cosmosId: Long, cluster: ClusterId): Boolean =
    store.withConnection { implicit c =>
      store.cluster.ownedBy(cosmosId).exists(_.clusterId == cluster)
    }
}

object ClusterResource extends Results {
  def clusterUrl(id: ClusterId)(implicit request: RequestHeader): String =
    AbsoluteUrl(routes.ClusterResource.listDetails(id.toString))

  private def unauthorizedAccessTo(cluster: ClusterId) =
    Unauthorized(Json.toJson(ErrorMessage(s"Cannot access cluster '$cluster'")))

  private def sharedClusterNoGroup = BadRequest(Json.toJson(ErrorMessage(
    "Cannot request a shared cluster because you don't belong to any group")))

  private def notFound(cluster: ClusterId) =
    NotFound(Json.toJson(ErrorMessage(s"No cluster '$cluster' exists")))
}
