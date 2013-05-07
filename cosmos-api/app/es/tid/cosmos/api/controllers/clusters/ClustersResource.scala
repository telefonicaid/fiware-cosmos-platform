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

package es.tid.cosmos.api.controllers.clusters

import scala.util.{Failure, Success, Try}

import play.Logger
import play.api.libs.json._
import play.api.mvc._

import es.tid.cosmos.api.controllers.common.{formatInternalException, JsonController}
import es.tid.cosmos.servicemanager.{ClusterId, ServiceManagerComponent}

/**
 * Resource that represents the whole set of clusters.
 */
trait ClustersResource extends JsonController {
  this: ServiceManagerComponent =>

  /**
   * List existing clusters.
   */
  def list = Action { implicit request =>
    val body = ClusterList(serviceManager.clusterIds.map(id => ClusterReference(id)))
    Ok(Json.toJson(body))
  }

  /**
   * Start a new cluster provisioning.
   */
  def createCluster = JsonBodyAction[CreateClusterParams] { (request, body) =>
    Try(serviceManager.createCluster(
      body.name, body.size, serviceManager.services)) match {
      case Success(id: ClusterId) => {
        Logger.info(s"Provisioning new cluster ${id}")
        val reference: ClusterReference = ClusterReference(id)(request)
        Created(Json.toJson(reference)).withHeaders(LOCATION -> reference.href)
      }
      case Failure(ex) => {
        val message = "Error when requesting a new cluster"
        Logger.error(message, ex)
        InternalServerError(formatInternalException(message, ex))
      }
    }
  }
}
