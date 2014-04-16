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

import scalaz._

import play.api.mvc.RequestHeader

import es.tid.cosmos.api.profile.ApiCredentials
import es.tid.cosmos.api.profile.ApiCredentials.{ApiKeyLength, ApiSecretLength}
import es.tid.cosmos.api.profile.dao.ProfileDataStore
import es.tid.cosmos.common.BasicAuth

/** Authenticated requests that have a BasicAuth header with the Cosmos API key and secret. */
private[request] class ApiCredentialsAuthentication(store: ProfileDataStore)
  extends RequestAuthentication {

  import Scalaz._

  /** Try to extract a cosmos profile from the request authentication headers.
    *
    * @param request  Request to authenticate
    * @return         Either a cosmos profile or a validation error
    */
  override def authenticateRequest(request: RequestHeader): AuthResult = for {
    credentials <- getApiCredentials(request)
    profile <- getProfileFromCredentials(credentials)
  } yield profile

  /** Either extract API credentials from request headers or get an error message. */
  private def getApiCredentials(request: RequestHeader): Validation[AuthError, ApiCredentials] = {
    val headers = request.headers
    val authorizationOpt = headers.get("Authorization")
    authorizationOpt match {
      case Some(BasicAuth(apiKey, apiSecret)) if isKeyPair(apiKey, apiSecret) =>
        ApiCredentials(apiKey, apiSecret).success
      case Some(malformedHeader: String) => MalformedAuthHeader(malformedHeader).failure
      case None => MissingAuthentication.failure
    }
  }

  private def isKeyPair(apiKey: String, apiSecret: String) =
    apiKey.length == ApiKeyLength && apiSecret.length == ApiSecretLength

  /** Either get the profile that owns the API credentials or an error message.
    *
    * @param credentials  Credentials to be checked against the DAO
    * @return             Either a cosmos profile or a validation error
    */
  private def getProfileFromCredentials(credentials: ApiCredentials): AuthResult =
    store.withConnection { implicit c =>
      store.profile.lookupByApiCredentials(credentials)
        .map(_.success)
        .getOrElse(InvalidAuthCredentials.failure)
    }
}
