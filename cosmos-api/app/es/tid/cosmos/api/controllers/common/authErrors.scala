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

package es.tid.cosmos.api.controllers.common

import es.tid.cosmos.api.profile.UserState._

abstract class AuthError(val message: String)
case object MalformedAuthHeader extends AuthError(
  s"Invalid API credentials: malformed authorization header")
case object MissingAuthentication extends AuthError("Missing authorization header")
case object InvalidAuthCredentials extends AuthError("Invalid API credentials")
case object InvalidProfileState {
  def apply(state: UserState): AuthError = {
    require(state != Enabled, "Enabled should not be an invalid state")
    state match {
      case Creating => new AuthError(
        "User profile is still being created. Hold on just a tad longer...") {}
      case Deleting => new AuthError("User profile is being deleted") {}
      case s@(Disabled | Deleted) => new AuthError(s"User profile is $s") {}
    }
  }
}
