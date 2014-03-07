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

import scalaz._

import play.api.mvc.RequestHeader

import es.tid.cosmos.api.controllers.common.BasicAuth
import es.tid.cosmos.api.profile.{ApiCredentials, CosmosDao}
import es.tid.cosmos.api.profile.ApiCredentials.{ApiKeyLength, ApiSecretLength}

/** Authenticated requests that have a BasicAuth header with the Cosmos API key and secret. */
private[request] class ApiCredentialsAuthentication(dao: CosmosDao)
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
    dao.store.withConnection { implicit c =>
      dao.profile.lookupByApiCredentials(credentials)
        .map(_.success)
        .getOrElse(InvalidAuthCredentials.failure)
    }
}
