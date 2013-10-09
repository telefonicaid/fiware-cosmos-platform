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

import scala.util.{Failure, Success, Try}

import play.api.Logger
import play.api.libs.json.Json
import play.api.mvc._

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.auth.ApiCredentials.{ApiKeyLength, ApiSecretLength}
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.controllers.pages.CosmosProfile
import es.tid.cosmos.api.profile.CosmosProfileDao

/**
 * Controller able to check authentication and authorization
 */
trait ApiAuthController extends Controller {
  val dao: CosmosProfileDao

  /**
   * Decorate a block with authentication header checks.
   *
   * Chain of checks:
   *  * Look for valid API key/secret pair
   *  * Look for a valid cookie
   *  * Issue an auth error response
   *
   * @param request    Request to extract credentials from
   * @param userAction Block able to create a response given a CosmosProfile
   * @return           Either userAction result or an authorization error response
   */
  def withApiAuth(request: Request[Any])(userAction: CosmosProfile => Result): Result = {
    val authenticateFromApiCredentials = for {
      credentials <- getApiCredentials(request)
      profile <- authorizeProfile(credentials)
    } yield profile

    (authenticateFromApiCredentials, authenticateFromSession(request)) match {
      case (Success(profile), _) => userAction(profile)
      case (_, Some(profile)) => userAction(profile)
      case (Failure(ex: AuthError), _) => {
        Logger.warn("Rejected API request", ex)
        unauthorizedResponse(ex)
      }
      case (Failure(ex), _) => throw ex
    }
  }

  /**
   * Either extract API credentials from request headers or get an error message.
   */
  private def getApiCredentials(request: Request[Any]): Try[ApiCredentials] =
    request.headers.get("Authorization") match {
      case Some(BasicAuth(apiKey, apiSecret))
        if apiKey.length == ApiKeyLength && apiSecret.length == ApiSecretLength =>
        Success(ApiCredentials(apiKey, apiSecret))
      case Some(malformedHeader: String) => Failure(MalformedAuthHeader)
      case _ => Failure(MissingAuthHeader)
    }

  /**
   * Either get the profile that owns the API credentials or an error message.
   */
  private def authorizeProfile(credentials: ApiCredentials): Try[CosmosProfile] =
    dao.withConnection { implicit c =>
      dao.lookupByApiCredentials(credentials)
        .map(Success(_))
        .getOrElse(Failure(InvalidAuthCredentials))
    }

  /**
   * Get the profile form the request session when possible.
   */
  def authenticateFromSession(request: RequestHeader): Option[CosmosProfile] = for {
    userId <- request.session.userId
    profile <- dao.withTransaction { implicit c =>
      dao.lookupByUserId(userId)
    }
  } yield profile

  private def unauthorizedResponse(error: AuthError) =
    Unauthorized(Json.toJson(ErrorMessage(error.getMessage)))
}
