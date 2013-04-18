package es.tid.cosmos.api.controllers

import play.api.mvc.{Action, Controller}
import play.api.libs.json.Json

import es.tid.cosmos.api.Components

/**
 * Root API resource
 *
 * @author sortega
 */
object Cosmos extends Controller with Components {

  val apiVersion = "1.0.0"

  def version = Action {
    Ok(Json.toJson(Map("version" -> apiVersion)))
  }
}
