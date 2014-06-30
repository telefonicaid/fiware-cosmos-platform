/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.api.controllers.profile

import com.wordnik.swagger.annotations._
import play.api.libs.json.Json
import play.api.mvc.{Controller, Action}

import es.tid.cosmos.api.auth.request.RequestAuthentication
import es.tid.cosmos.api.controllers._
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.common.auth.ApiAuthController
import es.tid.cosmos.api.profile.dao.ProfileDataStore

/** Resource that represents a user profile. */
@Api(value = "/cosmos/v1/profile", listingPath = "/doc/cosmos/v1/profile",
  description = "Represents the user profile")
class ProfileResource(override val auth: RequestAuthentication, store: ProfileDataStore)
  extends Controller with JsonController with ApiAuthController {

  /** Show user profile. */
  @ApiOperation(value = "Get the user profile details", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.profile.UserProfile")
  @ApiErrors(Array(new ApiError(code = 401, reason = "Unauthorized user")))
  def show = Action { implicit request =>
    for {
      profile <- requireAuthenticatedApiRequest(request)
    } yield Ok(Json.toJson(profile.toUserProfile))
  }

  /** Update the user profile */
  @ApiOperation(value = "Update the user profile details", httpMethod = "PUT",
    responseClass = "es.tid.cosmos.api.controllers.profile.UserProfile")
  @ApiErrors(Array(
    new ApiError(code = 400, reason = "Invalid JSON payload"),
    new ApiError(code = 400, reason = "No defined public key"),
    new ApiError(code = 400, reason = "More than one public key"),
    new ApiError(code = 400, reason = "Trying to modify user handle"),
    new ApiError(code = 401, reason = "Unauthorized user")
  ))
  @ApiParamsImplicit(Array(
    new ApiParamImplicit(paramType = "body",
      dataType = "es.tid.cosmos.api.controllers.profile.UserProfile")
  ))
  def update() = Action(parse.tolerantJson) { implicit request =>
    for {
      currentProfile <- requireAuthenticatedApiRequest(request)
      targetProfile <- validJsonBody[UserProfile](request)
    } yield {
      if (targetProfile.keys.length != 1) badRequest("Only one public key is supported")
      else store.withTransaction { implicit c =>
        if (currentProfile.handle != targetProfile.handle) {
          badRequest("Handle modification is not supported")
        } else {
          store.profile.setHandle(currentProfile.id, targetProfile.handle)
          store.profile.setEmail(currentProfile.id, targetProfile.email)
          store.profile.setPublicKeys(currentProfile.id, targetProfile.keys)
          Ok(Json.toJson(targetProfile))
        }
      }
    }
  }

  private def badRequest(message: String) = BadRequest(Json.toJson(Message(message)))
}
