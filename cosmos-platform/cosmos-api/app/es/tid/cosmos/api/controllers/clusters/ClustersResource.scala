package es.tid.cosmos.api.controllers.clusters

import scala.util.{Failure, Success, Try}

import play.api.libs.json._
import play.api.mvc._
import play.Logger

import es.tid.cosmos.servicemanager.{ClusterId, ServiceManagerComponent}
import es.tid.cosmos.api.controllers.common
import es.tid.cosmos.api.controllers.common.JsonController

/**
 * Resource that represents the whole set of clusters.
 *
 * @author sortega
 */
trait ClustersResource extends JsonController {
  self: ServiceManagerComponent =>

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
    Try(serviceManager.createCluster(body.name, body.size)) match {
      case Success(id: ClusterId) => {
        Logger.info(s"Provisioning new cluster ${id}")
        val reference: ClusterReference = ClusterReference(id)(request)
        Created(Json.toJson(reference)).withHeaders(LOCATION -> reference.href)
      }
      case Failure(ex) => {
        val message = "Error when requesting a new cluster"
        Logger.error(message, ex)
        InternalServerError(common.formatInternalException(message, ex))
      }
    }
  }
}
