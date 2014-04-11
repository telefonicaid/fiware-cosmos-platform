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

package es.tid.cosmos.api.controllers.infinity

import scalaz._

import com.typesafe.config.Config
import com.wordnik.swagger.annotations._
import play.api.libs.json.Json
import play.api.mvc.{Action, Controller, Results}

import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile.{ApiCredentials, ClusterSecret}
import es.tid.cosmos.api.profile.dao.{ClusterDataStore, ProfileDataStore}
import es.tid.cosmos.servicemanager.ServiceManager

@Api(value = "/infinity/v1/auth", listingPath = "/doc/infinity/v1/auth",
  description = "Authenticate users credentials for Infinity")
class InfinityAuthenticationResource(
    store: ProfileDataStore with ClusterDataStore,
    serviceManager: ServiceManager,
    config: Config) extends Controller {

  private val authenticator = new InfinityAuthenticator(store, serviceManager)
  private val requestAuthentication = new InfinityRequestAuthentication(config)

  import Scalaz._
  import InfinityAuthenticationResource._

  @ApiOperation(value = "Authenticate users credentials for Infinity", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.infinity.InfinityIdentity", notes ="""
      Lookup an identity from either an API pair (key and secret) or a cluster secret. Be careful
      to fill in the query string parameters for exactly one of the two options.""")
  @ApiErrors(Array(
    new ApiError(code = 400, reason = "If the identity can't be found"),
    new ApiError(code = 400,
      reason = "When other than just the pair API key/secret of cluster secret is present"),
    new ApiError(code = 401, reason = "Request lacks a basic authorization header"),
    new ApiError(code = 401, reason = "Invalid authentication credentials")
  ))
  def authenticate(
      @ApiParam(name="API key")
      apiKey: Option[String],
      @ApiParam(name="API secret")
      apiSecret: Option[String],
      @ApiParam(name="Cluster secret")
      clusterSecret: Option[String]) = Action { implicit request =>
    for {
      _ <- requestAuthentication.requireAuthorized(request)
      identity <- authenticateFromParameters(apiKey, apiSecret, clusterSecret)
    } yield Ok(Json.toJson(identity))
  }

  private def authenticateFromParameters(
      apiKey: Option[String],
      apiSecret: Option[String],
      clusterSecret: Option[String]): ActionValidation[InfinityIdentity] =
    (apiKey, apiSecret, clusterSecret) match {
      case (None, None, Some(secret)) => authenticateClusterSecret(secret)
      case (Some(key), Some(secret), None) => authenticateApiCredentials(key, secret)
      case _ => InvalidParametersResponse.failure
    }

  private def authenticateClusterSecret(secret: String): ActionValidation[InfinityIdentity] = for {
    clusterSecret <- exceptionAsBadRequest(ClusterSecret(secret))
    identity <- authenticator.authenticateClusterSecret(clusterSecret).leftMap(notFoundResponse)
  } yield identity

  private def authenticateApiCredentials(
      apiKey: String, apiSecret: String): ActionValidation[InfinityIdentity] = for {
    credentials <- exceptionAsBadRequest(ApiCredentials(apiKey, apiSecret))
    identity <- authenticator.authenticateApiCredentials(credentials).leftMap(notFoundResponse)
  } yield identity

  private def exceptionAsBadRequest[T](block: => T): ActionValidation[T] =
    Validation.fromTryCatch(block).leftMap(ex => BadRequest(Json.toJson(Message(ex.getMessage))))

  private def notFoundResponse(message: Message) = NotFound(Json.toJson(message))
}

private object InfinityAuthenticationResource extends Results {
  val InvalidParametersResponse = BadRequest(Json.toJson(Message("Invalid parameters. " +
    "Fill in either apiKey/apiSecret or clusterSecret but not both")))
}
