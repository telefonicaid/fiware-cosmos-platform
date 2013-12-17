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

package es.tid.cosmos.api.controllers.storage

import java.net.URI
import scala.language.postfixOps
import scalaz._

import com.wordnik.swagger.annotations.{ApiOperation, ApiErrors, ApiError, Api}
import play.api.libs.json.Json
import play.api.mvc.Action

import es.tid.cosmos.api.controllers.admin.MaintenanceStatus
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile.CosmosProfileDao
import es.tid.cosmos.servicemanager.ServiceManager

/**
 * Resource that represents the persistent HDFS shared by all clusters.
 */
@Api(value = "/cosmos/v1/storage", listingPath = "/doc/cosmos/v1/storage",
  description = "Represents the persistent storage shared by all clusters")
class StorageResource(
    serviceManager: ServiceManager,
    override val dao: CosmosProfileDao,
    override val maintenanceStatus: MaintenanceStatus)
  extends ApiAuthController with MaintenanceAwareController {
  
  import Scalaz._

  private val UnavailableHdfsResponse =
    ServiceUnavailable(Json.toJson(ErrorMessage("persistent storage service not available")))

  /**
   * Returns HDFS connection details.
   */
  @ApiOperation(value = "Persistent storage connection details", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.storage.WebHdfsConnection")
  @ApiErrors(Array(
    new ApiError(code = 503, reason = "When service is under maintenance")
  ))
  def details() = Action { implicit request =>
    for {
      _ <- requireResourceNotUnderMaintenance()
      profile <- requireAuthenticatedApiRequest(request)
      location <- persistentWebHdfsUri().toSuccess(UnavailableHdfsResponse)
    } yield Ok(Json.toJson(WebHdfsConnection(location, profile.handle)))
  }
  
  private def persistentWebHdfsUri(): Option[URI] = for {
    description <- serviceManager.describeCluster(serviceManager.persistentHdfsId)
    if description.nameNode.isDefined
  } yield toWebHdfsUri(description.nameNode.get)

  private def toWebHdfsUri(nameNode: URI): URI =
    new URI("webhdfs", null, nameNode.getHost, nameNode.getPort, null, null, null)
}
