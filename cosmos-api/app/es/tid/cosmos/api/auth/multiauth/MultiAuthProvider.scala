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

package es.tid.cosmos.api.auth.multiauth

import es.tid.cosmos.api.auth.AuthProvider
import es.tid.cosmos.api.auth.oauth2.OAuthProvider

trait MultiAuthProvider {

  val providers: Map[String, AuthProvider]

  /** When enabled, the OAuth 2 provider used for token-based authentication. */
  val tokenAuthenticationProvider: Option[OAuthProvider]

  def oauthProviders: Map[String, OAuthProvider] = providers.collect {
    case (id, provider: OAuthProvider) => (id, provider)
  }
}
