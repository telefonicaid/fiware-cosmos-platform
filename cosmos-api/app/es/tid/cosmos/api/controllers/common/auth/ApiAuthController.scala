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
  private def enabledProfile(profile: CosmosProfile): Validation[AuthError, CosmosProfile] = {
    import Scalaz._
    if (profile.state == UserState.Enabled) profile.success
    else InvalidProfileState(profile.state).failure
  }

  private def unauthorizedResponse(error: AuthError) =
    Unauthorized(Json.toJson(ErrorMessage(error.message)))
}
