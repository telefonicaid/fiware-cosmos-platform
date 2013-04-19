package es.tid.cosmos.api.controllers

import play.api.mvc.{Action, Controller}
import play.api.libs.json.Json

/**
 * Root API resource
 *
 * @author sortega
 */
trait Cosmos {
  self: Controller =>

  val apiVersion = "1.0.0"

  def version = Action {
    Ok(Json.toJson(Map("version" -> apiVersion)))
  }
}
