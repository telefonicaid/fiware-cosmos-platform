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

import scala.Some
import scala.util.{Failure, Success, Try}

import play.api.libs.json._
import play.api.mvc.{RequestHeader, Action, Controller}

import es.tid.cosmos.api.controllers.common.Message
import es.tid.cosmos.api.controllers.routes
import es.tid.cosmos.servicemanager.{ServiceManagerComponent, ClusterId, ClusterDescription}

/**
 * Resource that represents a single cluster.
 */
trait ClusterResource extends Controller {
  this: ServiceManagerComponent =>

  implicit object ClusterDescriptionWrites extends Writes[ClusterDescription] {
    def writes(desc: ClusterDescription): JsValue = Json.obj(
      "id" -> desc.id.toString,
      "name" -> desc.name,
      "size" -> desc.size,
      "state" -> desc.state.name,
      "state_description" -> desc.state.descLine
    )
  }

  def listDetails(id: String) = Action {
    serviceManager.describeCluster(ClusterId(id)) match {
      case Some(description) => Ok(Json.toJson(description))
      case None => NotFound(Json.toJson(Message(s"No cluster '$id' exists")))
    }
  }

  def terminate(id: String) = Action {
    Try(serviceManager.terminateCluster(ClusterId(id))) match {
      case Success(_) => Ok(Json.toJson(Message("Terminating cluster")))
      case Failure(ex) => InternalServerError(ex.getMessage)
    }
  }
}

object ClusterResource {
  def clusterUrl(id: ClusterId)(implicit request: RequestHeader): String =
    routes.Application.listDetails(id.toString).absoluteURL(secure = false)
}
