package es.tid.cosmos.api.controllers

import play.api.libs.json._
import play.api.mvc.{RequestHeader, Action, Controller}
import scala.Some
import scala.util.{Failure, Success, Try}

import es.tid.cosmos.servicemanager.{ClusterId, ClusterDescription}
import es.tid.cosmos.api.sm.ServiceManagerComponent

/**
 * @author sortega
 */
trait ClusterResource {
  self: Controller with ServiceManagerComponent =>

  implicit object ClusterDescriptionWrites extends Writes[ClusterDescription] {
    def writes(desc: ClusterDescription): JsValue = JsObject(Seq(
      "id" -> JsString(desc.id.toString),
      "name" -> JsString(desc.name),
      "size" -> JsNumber(desc.size),
      "state" -> JsString(desc.state.name),
      "state_description" -> JsString(desc.state.descLine)
    ))
  }

  def listDetails(id: String) = Action {
    serviceManager.describeCluster(ClusterId(id)) match {
      case Some(description) => Ok(Json.toJson(description))
      case None => NotFound(s"No cluster '${id}' exists")
    }
  }

  def terminate(id: String) = Action {
    Try(serviceManager.terminateCluster(ClusterId(id))) match {
      case Success(_) => Ok("Terminating cluster")
      case Failure(ex) => InternalServerError(ex.getMessage)
    }
  }
}

object ClusterResource {
  def clusterUrl(id: ClusterId)(implicit request: RequestHeader): String =
    routes.Application.listDetails(id.toString).absoluteURL(secure = false)
}
