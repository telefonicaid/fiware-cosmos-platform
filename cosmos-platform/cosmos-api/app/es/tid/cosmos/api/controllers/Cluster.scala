package es.tid.cosmos.api.controllers

import play.api.libs.json._
import play.api.mvc.{RequestHeader, Action, Controller}
import scala.Some
import scala.util.{Failure, Success, Try}

import es.tid.cosmos.api.Components
import es.tid.cosmos.sm.ClusterDescription

/**
 * @author sortega
 */
object Cluster extends Controller with Components {

  implicit object ClusterDescriptionWrites extends Writes[ClusterDescription] {
    def writes(desc: ClusterDescription): JsValue = JsObject(Seq(
      "id" -> JsString(desc.id),
      "name" -> JsString(desc.name),
      "size" -> JsNumber(desc.size),
      "state" -> JsString(desc.state.name),
      "state_description" -> JsString(desc.state.descLine)
    ))
  }

  def listDetails(id: String) = Action {
    serviceManager.describeCluster(id) match {
      case Some(description) => Ok(Json.toJson(description))
      case None => NotFound(s"No cluster '${id}' exists")
    }
  }

  def terminate(id: String) = Action {
    Try(serviceManager.terminateCluster(id)) match {
      case Success(_) => Ok("Terminating cluster")
      case Failure(ex) => InternalServerError(ex.getMessage)
    }
  }

  def clusterUrl(id: String)(implicit request: RequestHeader) =
    routes.Cluster.listDetails(id).absoluteURL(secure = false)
}
