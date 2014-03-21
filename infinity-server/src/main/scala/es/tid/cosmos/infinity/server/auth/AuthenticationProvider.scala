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

package es.tid.cosmos.infinity.server.auth

object AuthenticationProvider {

  /** A message to request the authentication of the given credentials. */
  case class Authenticate(credentials: Credentials)

  /** A message to respond with a valid authentication request. */
  case class Authenticated(profile: UserProfile)

  /** A message to response with a failed authentication request. */
  case class AuthenticationFailed(error: Throwable)
}
