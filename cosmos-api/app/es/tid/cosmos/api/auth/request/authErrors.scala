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
