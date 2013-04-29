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

package es.tid.cosmos.api.controllers

import scala.util.{Failure, Success, Try}

import play.api.libs.json._
import play.api.libs.functional.syntax._
import play.api.mvc.{SimpleResult, Action, Controller}

import es.tid.cosmos.servicemanager.{ServiceManagerComponent, ClusterId}

/**
 * @author sortega
 */
package object clusters {

  case class CreateClusterParams(name: String, size: Int)

  implicit val jsonReader: Reads[CreateClusterParams] = (
    (__ \ "name").read[String] ~
    (__ \ "size").read[Int]
  )(CreateClusterParams.apply _)

  implicit object CreateClusterParamsWriter extends Writes[CreateClusterParams] {
    def writes(params: CreateClusterParams) = Json.obj(
      "name" -> JsString(params.name),
      "size" -> JsNumber(params.size)
    )
  }

  trait ClustersResource {
    self: Controller with ServiceManagerComponent =>

    def list = Action { implicit request =>
      Ok(Json.toJson(Map("clusters" -> serviceManager.clusterIds.map(id =>
        Map("id" -> id.toString, "href" -> ClusterResource.clusterUrl(id))))))
    }

    def createCluster = Action(parse.tolerantJson) { implicit request =>
      Json.fromJson[CreateClusterParams](request.body) match {
        case JsSuccess(params, _) => Try(serviceManager.createCluster(params.name, params.size)) match {
          case Success(id: ClusterId) => {
            val url = ClusterResource.clusterUrl(id)
            Created(Json.toJson(Map("id" -> id.uuid.toString, "href" -> url)))
              .withHeaders(LOCATION -> url)
          }
          case Failure(ex) => BadRequest(Json.toJson(Map(
            "error" -> ex.getMessage,
            "exception" -> ex.getClass.getCanonicalName,
            "stacktrace" -> ex.getStackTraceString)))
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
}
