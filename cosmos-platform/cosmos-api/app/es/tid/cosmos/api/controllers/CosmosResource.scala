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
