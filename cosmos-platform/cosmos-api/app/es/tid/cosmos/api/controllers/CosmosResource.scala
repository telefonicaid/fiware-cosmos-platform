package es.tid.cosmos.api.controllers

import play.api.mvc.{Action, Controller}
import play.api.libs.json.Json

/**
 * Root API resource
 *
 * @author sortega
 */
trait CosmosResource {
  self: Controller =>

  def version = Action {
    Ok(Json.toJson(Map("version" -> CosmosResource.apiVersion)))
  }
}

object CosmosResource {
  val apiVersion = "1.0.0"
}