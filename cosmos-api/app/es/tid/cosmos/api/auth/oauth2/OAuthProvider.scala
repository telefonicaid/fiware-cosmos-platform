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

import scala.concurrent.Future

import es.tid.cosmos.api.auth.AuthProvider

trait OAuthProvider extends AuthProvider {

  /** Human-readable name of the provider */
  def name: String

  /** Create an OAuth authentication URL that asks for the required scopes.
    *
    * @param redirectUrl  URI that the OAuth provider uses to return the control
    * @return             The URL
    */
  def authenticationUrl(redirectUrl: String): String

  /**Link to the sign up page when supported by the provider. */
  def newAccountUrl: Option[String]

  /** Request a token able to access the user profile.
    *
    * @param code         Authorization code
    * @param redirectUrl  URI that the OAuth provider uses to return the control
    * @return             Future access token or an OAuthException in case of error
    */
  def requestAccessToken(code: String, redirectUrl: String): Future[String]

  /** Request the user profile given an access token
    *
    * @param token  Access token
    * @return       Future user profile, an [[OAuthException]] in case of OAuth error or
    *               [[InvalidProfileException]] if the profile cannot be parsed.
    */
  def requestUserProfile(token: String): Future[OAuthUserProfile]
}
