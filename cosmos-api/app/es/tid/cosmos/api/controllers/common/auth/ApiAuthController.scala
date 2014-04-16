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

package es.tid.cosmos.api.controllers.common.auth

import scalaz._

import play.api.Logger
import play.api.libs.json.Json
import play.api.mvc._

import es.tid.cosmos.api.auth.request._
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile.{UserState, Capability, CosmosProfile}

/** Controller-mixin able to check authentication and authorization.
  *
  * Must be mixed-in onto Controllers with a field of type CosmosProfileDao
  */
trait ApiAuthController { this: Controller =>

  import Scalaz._

  /** Authentication mechanism to build requirements on. */
  protected val auth: RequestAuthentication

  /** Require an API request to be authenticated.
    *
    * @param request   Request to extract credentials from
    * @return          Either a user profile or an authorization error response
    */
  def requireAuthenticatedApiRequest(request: RequestHeader): ActionValidation[CosmosProfile] =
    auth.authenticateRequest(request).flatMap(enabledProfile).leftMap { error =>
      Logger.warn(s"Rejected API request: ${error.message}")
      unauthorizedResponse(error)
    }

  /** Require an API request authenticated as in `requireAuthenticatedApiRequest` and also
    * require that the credentials are from an actual operator.
    *
    * @param request   Request to extract credentials from
    * @return          Either a user profile or an authorization error response
    */
  def requireOperatorApiRequest(request: RequestHeader): ActionValidation[CosmosProfile] = for {
    profile <- requireAuthenticatedApiRequest(request)
    _ <- if (profile.capabilities.hasCapability(Capability.IsOperator)) ().success
         else Forbidden(Json.toJson(Message("You need to be an operator"))).failure
  } yield profile

  /** Check for enabled profiles.
    *
    * @param profile  Profile that must be enabled
    * @return         Either an enabled profile or a validation error
    */
  private def enabledProfile(profile: CosmosProfile): Validation[AuthError, CosmosProfile] =
    if (profile.state == UserState.Enabled) profile.success
    else InvalidProfileState(profile.state).failure

  private def unauthorizedResponse(error: AuthError) =
    Unauthorized(Json.toJson(ErrorMessage(error.message)))
}
