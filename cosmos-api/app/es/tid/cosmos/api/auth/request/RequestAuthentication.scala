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

package es.tid.cosmos.api.auth.request

import scalaz._

import play.api.mvc.RequestHeader

import es.tid.cosmos.api.profile.{UserState, CosmosProfile}

/** Authentication scheme */
trait RequestAuthentication {
  type AuthResult = Validation[AuthError, CosmosProfile]

  /** Authenticates a request.
    *
    * @param request  Request to authenticate
    * @return         Profile of the user performing the request or an authentication error
    */
  def authenticateRequest(request: RequestHeader): AuthResult

  /** Check for enabled profiles.
    *
    * @param profile  Profile that must be enabled
    * @return         Either an enabled profile or a validation error
    */
  protected def enabledProfile(profile: CosmosProfile): AuthResult = {
    import Scalaz._
    if (profile.state == UserState.Enabled) profile.success
    else InvalidProfileState(profile.state).failure
  }
}
