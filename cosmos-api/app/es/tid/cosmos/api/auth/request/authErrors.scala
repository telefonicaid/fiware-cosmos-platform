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

import es.tid.cosmos.api.profile.UserState._

sealed abstract class AuthError(val message: String) {
  override def toString = message
}

case class MalformedAuthHeader(malformedHeader: String) extends AuthError(
  s"Invalid API credentials: malformed authorization header '$malformedHeader'")

case object MissingAuthentication extends AuthError("Missing authorization header")

case object InvalidAuthCredentials extends AuthError("Invalid API credentials")

case class InvalidProfileState(state: UserState)
  extends AuthError(InvalidProfileState.messageForState(state))

case object InvalidProfileState {
  private def messageForState(state: UserState): String = {
    state match {
      case Creating => "User profile is still being created. Hold on just a tad longer..."
      case Deleting => "User profile is being deleted"
      case s@(Disabled | Deleted) => s"User profile is $s"
      case Enabled => throw new IllegalArgumentException("'Enabled' is not an invalid state")
    }
  }
}

case class CannotRetrieveProfile(cause: Throwable)
  extends AuthError(s"Cannot retrieve user profile: $cause")
