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

package es.tid.cosmos.api.auth.multiauth

import es.tid.cosmos.api.auth.AuthProvider
import es.tid.cosmos.api.auth.oauth2.OAuthProvider

trait MultiAuthProvider {

  val providers: Map[String, AuthProvider]

  /** When enabled, the OAuth 2 provider used for token-based authentication.
    * It must be one of the providers on the ``providers`` val.
    */
  val tokenAuthenticationProvider: Option[OAuthProvider]

  def oauthProviders: Map[String, OAuthProvider] = providers.collect {
    case (id, provider: OAuthProvider) => (id, provider)
  }
}
