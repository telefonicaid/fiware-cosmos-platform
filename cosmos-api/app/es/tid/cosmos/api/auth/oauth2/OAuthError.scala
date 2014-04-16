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

package es.tid.cosmos.api.auth.oauth2

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
