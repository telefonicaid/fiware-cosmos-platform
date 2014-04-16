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

package es.tid.cosmos.api.controllers.storage

import java.net.URI
import scala.language.postfixOps
import scalaz._

import com.wordnik.swagger.annotations.{ApiOperation, ApiErrors, ApiError, Api}
import play.api.libs.json.Json
import play.api.mvc.Action

import es.tid.cosmos.api.controllers.admin.MaintenanceStatus
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.common.auth.ApiAuthController
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.api.auth.request.RequestAuthentication

/** Resource that represents the persistent HDFS shared by all clusters. */
@Api(value = "/cosmos/v1/storage", listingPath = "/doc/cosmos/v1/storage",
  description = "Represents the persistent storage shared by all clusters")
class StorageResource(
    override val auth: RequestAuthentication,
    serviceManager: ServiceManager,
    override val maintenanceStatus: MaintenanceStatus)
  extends ApiAuthController with MaintenanceAwareController {

  import Scalaz._

  private val UnavailableHdfsResponse =
    ServiceUnavailable(Json.toJson(ErrorMessage("persistent storage service not available")))

  /** Returns HDFS connection details. */
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
