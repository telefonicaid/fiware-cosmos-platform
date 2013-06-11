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

package es.tid.cosmos.api.controllers.hdfs

import java.net.URI

import com.wordnik.swagger.annotations.{ApiOperation, Api}
import play.api.libs.json.Json
import play.api.mvc.Action

import es.tid.cosmos.api.controllers.common.{ErrorMessage, AuthController}
import es.tid.cosmos.servicemanager.ServiceManager

/**
 * Resource that represents the persistent HDFS shared by all clusters.
 */
@Api(value = "/cosmos/hdfs", listingPath = "/doc/cosmos/hdfs",
  description = "Represents the persistent HDFS shared by all clusters")
class PersistentHdfsResource(serviceManager: ServiceManager) extends AuthController {

  private val UnavailableHdfsResponse =
    ServiceUnavailable(Json.toJson(ErrorMessage("HDFS service not available")))

  /**
   * Returns HDFS connection details.
   */
  @ApiOperation(value = "Persistent WebHDFS connection details", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.hdfs.WebHdfs")
  def details() = Action { implicit request =>
    Authenticated(request) { profile =>
      (for {
        description <- serviceManager.describeCluster(serviceManager.persistentHdfsId)
        webHdfsUri = toWebHdfsUri(description.nameNode)
      } yield Ok(Json.toJson(WebHdfsConnection(location = webHdfsUri, user = profile.handle))))
      .getOrElse(UnavailableHdfsResponse)
    }
  }

  private def toWebHdfsUri(nameNode: URI): URI =
    new URI("webhdfs", null, nameNode.getHost, nameNode.getPort, null, null, null)
}
