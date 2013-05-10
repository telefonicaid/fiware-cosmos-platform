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
import scala.Some
import scala.util.{Failure, Success, Try}

import com.wordnik.swagger.annotations._
import play.api.libs.json._
import play.api.mvc.{RequestHeader, Action, Controller}

import es.tid.cosmos.api.controllers.common.Message
import es.tid.cosmos.servicemanager.{ServiceManager, ClusterId}

/**
 * Resource that represents a single cluster.
 */
@Api(value = "/cosmos/cluster", listingPath = "/doc/cosmos/cluster",
  description = "Represents an existing or decommissioned cluster")
class ClusterResource(serviceManager: ServiceManager) extends Controller {
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
    serviceManager.describeCluster(ClusterId(id)) match {
      case Some(description) => Ok(Json.toJson(ClusterDetails(description)))
      case None => NotFound(Json.toJson(Message(s"No cluster '$id' exists")))
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
       id: String) = Action {
    Try(serviceManager.terminateCluster(ClusterId(id))) match {
      case Success(_) => Ok(Json.toJson(Message("Terminating cluster")))
      case Failure(ex) => InternalServerError(ex.getMessage)
    }
  }
}

object ClusterResource {
  def clusterUrl(id: ClusterId)(implicit request: RequestHeader): String =
    routes.ClusterResource.listDetails(id.toString).absoluteURL(secure = false)
}
