package es.tid.cosmos.api.controllers

import scala.util.{Failure, Success, Try}

import play.api.libs.json._
import play.api.libs.functional.syntax._
import play.api.mvc.{SimpleResult, Action, Controller}

import es.tid.cosmos.api.Components

/**
 * @author sortega
 */
object Clusters extends Controller with Components {

  case class CreateClusterParams(name: String, size: Int)

  object CreateClusterParams {
    implicit val jsonReader = (
      (__ \ "name").read[String] ~
      (__ \ "size").read[Int]
    )(CreateClusterParams.apply _)
  }

  def list = Action { implicit request =>
    Ok(Json.toJson(Map("clusters" -> serviceManager.clusterIds.map(id =>
      Map("id" -> id, "href" -> Cluster.clusterUrl(id))))))
  }

  def createCluster = Action(parse.tolerantJson) { implicit request =>
    Json.fromJson[CreateClusterParams](request.body) match {
      case JsSuccess(params, _) => Try(serviceManager.createCluster(params.name, params.size)) match {
        case Success(id) => {
          val url = Cluster.clusterUrl(id)
          Created(Json.toJson(Map("id" -> id, "href" -> url))).withHeaders(LOCATION -> url)
        }
        case Failure(ex) => BadRequest(Json.toJson(Map("error" -> ex.getMessage)))
      }
      case error @ JsError(_) => formatJsError(error)
    }
  }

  def formatJsError(jsError: JsError): SimpleResult[JsValue] = {
    val errorMap = Map(jsError.errors.toList.map {
      case (path, errors) => (path.toString(), errors.map(_.message).toList)
    }: _*)
    BadRequest(Json.toJson(errorMap))
  }
}
