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

package es.tid.cosmos.infinity.server.authentication

import java.net.InetAddress

import es.tid.cosmos.infinity.common.permissions.UserProfile

/** An exception thrown when a request cannot be authenticated. */
class AuthenticationException(msg: String, cause: Throwable = null) extends Exception(msg, cause)

object AuthenticationException {
  def cannotAccessService(cause: Throwable) =
    new AuthenticationException("Cannot access the authentication service", cause)

  def invalidProfile(profile: String, cause: Throwable) = new AuthenticationException(
    "Cannot parse profile received from the authentication service, " +
      s"check that all deployed services are compatible. Profile:\n$profile", cause)

  def invalidOrigin(origin: InetAddress, profile: UserProfile) = new AuthenticationException(
    s"Invalid access from origin '$origin' to '${profile.username}' profile")

  def authenticationRejected(cause: Throwable) = new AuthenticationException(
    "Authentication was rejected. Please check that infinity secret is correctly " +
      "configured for all services.", cause)
}
