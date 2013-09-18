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

package es.tid.cosmos.api.controllers.profile

import com.wordnik.swagger.annotations.{Api, ApiOperation}
import play.api.libs.json.Json
import play.api.mvc.Action

import es.tid.cosmos.api.controllers.common.{AuthController, JsonController}
import es.tid.cosmos.api.profile.CosmosProfileDao

/**
 * Resource that represents a user profile.
 */
@Api(value = "/cosmos/v1/profile", listingPath = "/doc/cosmos/v1/profile",
  description = "Represents the user profile")
class ProfileResource(override val dao: CosmosProfileDao)
  extends JsonController with AuthController {

  /**
   * List user clusters.
   */
  @ApiOperation(value = "Get the user profile details", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.profile.UserProfile")
  def show = Action { implicit request =>
    Authenticated(request) { profile =>
      Ok(Json.toJson(UserProfile(profile.handle, profile.keys.toList)))
    }
  }
}
