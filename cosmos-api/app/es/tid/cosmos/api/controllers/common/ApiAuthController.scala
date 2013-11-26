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

package es.tid.cosmos.api.controllers.common

import scalaz._

import play.api.Logger
import play.api.libs.json.Json
import play.api.mvc._

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.auth.ApiCredentials.{ApiKeyLength, ApiSecretLength}
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.profile.{UserState, CosmosProfile, CosmosProfileDao}

/**
 * Controller able to check authentication and authorization.
 *
 * Must be mixed-in onto Controllers with a field of type CosmosProfileDao
 */
trait ApiAuthController extends Controller {

  import Scalaz._

  val dao: CosmosProfileDao

  /** Require an API request to be authenticated either by an API key or
    * by a session cookie.
    *
    * Note: session cookies are supported for cosmos users convenience as it let them to
    * experiment on the Swagger documentation with real requests.
    *
    * @param request   Request to extract credentials from
    * @return          Either a user profile or an authorization error response
    */
  def requireAuthenticatedApiRequest(request: RequestHeader): ActionValidation[CosmosProfile] =
    selectAuthentication(
      preferredAuth = authenticateFromApiCredentials(request),
      fallbackAuth = authenticateFromSession(request)
    ).leftMap(error => {
      Logger.warn(s"Rejected API request: ${error.message}")
      unauthorizedResponse(error)
    })

  /** Select one out of two authentications preferring the first one.
    * When the first authentication succeed, the second one is not evaluated at all.
    *
    * @param preferredAuth  Preferred authentication
    * @param fallbackAuth   Fallback authentication that might not be evaluated
    * @return               Selected authentication
    */
  private def selectAuthentication(
      preferredAuth: Validation[AuthError, CosmosProfile],
      fallbackAuth: => Validation[AuthError, CosmosProfile]) =
    if (preferredAuth.isSuccess) preferredAuth
    else if (fallbackAuth.isSuccess) fallbackAuth
    else preferredAuth

  /**
   * Try to extract a cosmos profile from the request authentication headers.
   *
   * @param request  Request to authenticate
   * @return         Either a cosmos profile or a validation error
   */
  private def authenticateFromApiCredentials(
      request: RequestHeader): Validation[AuthError, CosmosProfile] = {
    for {
      credentials <- getApiCredentials(request)
      profile <- getProfileFromCredentials(credentials)
      _ <- enabledProfile(profile)
    } yield profile
  }

  /** Either extract API credentials from request headers or get an error message. */
  private def getApiCredentials(request: RequestHeader): Validation[AuthError, ApiCredentials] =
    request.headers.get("Authorization") match {
      case Some(BasicAuth(apiKey, apiSecret)) if isKeyPair(apiKey, apiSecret) =>
        ApiCredentials(apiKey, apiSecret).success
      case Some(malformedHeader: String) => MalformedAuthHeader.failure
      case _ => MissingAuthentication.failure
    }

  private def isKeyPair(apiKey: String, apiSecret: String) =
    apiKey.length == ApiKeyLength && apiSecret.length == ApiSecretLength

  /** Either get the profile that owns the API credentials or an error message.
    *
    * @param credentials  Credentials to be checked against the DAO
    * @return             Either a cosmos profile or a validation error
    */
  private def getProfileFromCredentials(
      credentials: ApiCredentials): Validation[AuthError, CosmosProfile] =
    dao.withConnection { implicit c =>
      dao.lookupByApiCredentials(credentials)
        .map(_.success)
        .getOrElse(InvalidAuthCredentials.failure)
    }

  /** Get the profile form the request session when possible. */
  private def authenticateFromSession(request: RequestHeader): Validation[AuthError, CosmosProfile] =
    for {
      profile <- getProfileFromSession(request)
      _ <- enabledProfile(profile)
    } yield profile

  /** Try to extract a cosmos profile from a session cookie.
    *
    * @param request Request whose session is inspected
    * @return        Either a cosmos profile or a validation error
    */
  private def getProfileFromSession(request: RequestHeader): Validation[AuthError, CosmosProfile] =
    (for {
      userId <- request.session.userId
      profile <- dao.withTransaction { implicit c =>
        dao.lookupByUserId(userId)
      }
    } yield profile.success).getOrElse(MissingAuthentication.failure)

  /** Check for enabled profiles.
    *
    * @param profile  Profile that must be enabled
    * @return         Either an enabled profile or a validation error
    */
  private def enabledProfile(profile: CosmosProfile): Validation[AuthError, CosmosProfile] =
    if (profile.state == UserState.Enabled) profile.success
    else InvalidAuthCredentials.failure

  private def unauthorizedResponse(error: AuthError) =
    Unauthorized(Json.toJson(ErrorMessage(error.message)))
}
