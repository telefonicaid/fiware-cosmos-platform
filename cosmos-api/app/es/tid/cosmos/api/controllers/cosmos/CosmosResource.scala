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

package es.tid.cosmos.api.controllers.cosmos

import com.typesafe.config.ConfigFactory
import com.wordnik.swagger.annotations.{ApiOperation, Api}
import play.api.libs.json.Json
import play.api.mvc.{Action, Controller}

/**
 * Root API resource
 */
@Api(value = "/cosmos/v1", listingPath = "/doc/cosmos/v1", description = "API description")
class CosmosResource extends Controller {
  @ApiOperation(value = "Get API version", httpMethod = "GET",
    notes = "Root API resource with version information",
    responseClass = "es.tid.cosmos.api.controllers.cosmos.ApiDescription")
  def version = Action {
    Ok(Json.toJson(CosmosResource.apiDescription))
  }
}

object CosmosResource {
  lazy val apiVersion = ConfigFactory.load().getString("api.version")
  lazy val apiDescription = ApiDescription(apiVersion)
}
