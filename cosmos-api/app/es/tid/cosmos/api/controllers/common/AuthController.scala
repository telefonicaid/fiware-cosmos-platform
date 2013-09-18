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

import es.tid.cosmos.api.authorization.ApiCredentials
import es.tid.cosmos.api.authorization.ApiCredentials.{ApiKeyLength, ApiSecretLength}
import es.tid.cosmos.api.controllers.pages.CosmosProfile
import es.tid.cosmos.api.profile.CosmosProfileDao

/**
 * Controller able to check authentication and authorization
 */
trait AuthController extends Controller {
  val dao: CosmosProfileDao

  /**
   * Decorate a block with authentication header checks.
   *
   * @param request    Request to extract credentials from
   * @param userAction Block able to create a response given a CosmosProfile
   * @return           Either userAction result or an authorization error response
   */
  def Authenticated(request: Request[Any])(userAction: CosmosProfile => Result): Result =
    (for {
      credentials <- getApiCredentials(request)
      profile <- authorizeProfile(credentials)
    } yield profile) match {
      case Success(profile) => userAction(profile)
      case Failure(ex: AuthError) => unauthorizedResponse(ex)
      case Failure(ex) => throw ex
    }

  /**
   * Either extract API credentials from request headers or get an error message.
   */
  private def getApiCredentials(request: Request[Any]): Try[ApiCredentials] =
    request.headers.get("Authorization") match {
      case Some(BasicAuth(apiKey, apiSecret))
        if apiKey.length == ApiKeyLength && apiSecret.length == ApiSecretLength =>
        Success(ApiCredentials(apiKey, apiSecret))
      case Some(malformedHeader: String) => {
        Logger.warn(s"Rejected request with malformed authentication header: $malformedHeader")
        Failure(MalformedAuthHeader)
      }
      case _ => {
        Logger.warn("Rejected request with missing authentication header")
        Failure(MissingAuthHeader)
      }
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

  private def unauthorizedResponse(error: AuthError) =
    Unauthorized(Json.toJson(ErrorMessage(error.getMessage)))
}
