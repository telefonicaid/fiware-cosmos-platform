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

package es.tid.cosmos.api.oauth2

/**
 * OAuth 2 errors as listed in RFC 6749 section 5.2.
 */
object OAuthError extends Enumeration {
  type OAuthError = Value
  val InvalidRequest = Value("invalid_request")
  val InvalidClient = Value("invalid_client")
  val InvalidGrant = Value("invalid_grant")
  val UnauthorizedClient = Value("unauthorized_client")
  val UnsupportedGrantType = Value("unsupported_grant_type")
  val InvalidScope = Value("invalid_scope")

  def parse(str: String): Option[OAuthError] =
    OAuthError.values.find(value => value.toString == str)
}
