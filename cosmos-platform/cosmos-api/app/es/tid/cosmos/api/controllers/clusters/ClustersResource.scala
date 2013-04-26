package es.tid.cosmos.api.controllers.clusters

import scala.util.{Failure, Success, Try}

import play.api.libs.json.{JsError, Json, JsSuccess}
import play.api.mvc.{Action, Controller}
import play.Logger

import es.tid.cosmos.api.controllers.{formatJsError, formatInternalException}
import es.tid.cosmos.servicemanager.{ClusterId, ServiceManagerComponent}
import es.tid.cosmos.api.controllers.cluster.ClusterResource

/**
 * Resource that represents the whole set of clusters.
 *
 * @author sortega
 */
trait ClustersResource {
  self: Controller with ServiceManagerComponent =>

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
  def createCluster = Action(parse.tolerantJson) { implicit request =>
    Json.fromJson[CreateClusterParams](request.body) match {
      case JsSuccess(params, _) => Try(serviceManager.createCluster(params.name, params.size)) match {
        case Success(id: ClusterId) => {
          Logger.info("Provisioning new cluster " + id)
          Created(Json.toJson(ClusterReference(id)))
            .withHeaders(LOCATION -> ClusterResource.clusterUrl(id))
        }
        case Failure(ex) => {
          val message = "Error when requesting a new cluster"
          Logger.error(message, ex)
          InternalServerError(formatInternalException(message, ex))
        }
      }
      case error@JsError(_) => BadRequest(formatJsError(error))
    }
  }
}
