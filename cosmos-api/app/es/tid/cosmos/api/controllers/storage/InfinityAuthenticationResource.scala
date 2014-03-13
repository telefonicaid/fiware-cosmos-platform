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

package es.tid.cosmos.api.controllers.storage

import scalaz._

import com.wordnik.swagger.annotations._
import play.api.libs.json.Json
import play.api.mvc.{Controller, Action}

import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile.dao.{ClusterDataStore, ProfileDataStore}
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.api.profile.{ApiCredentials, ClusterSecret}

@Api(value = "/cosmos/v1/storage/auth", listingPath = "/doc/cosmos/v1/storage/auth",
  description = "Authenticate users credentials for Infinity")
class InfinityAuthenticationResource(
    store: ProfileDataStore with ClusterDataStore,
    serviceManager: ServiceManager) extends Controller {

  private val authenticator = new InfinityAuthenticator(store, serviceManager)

  import Scalaz._

  @ApiOperation(value = "Authenticate users credentials for Infinity", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.storage.InfinityIdentity", notes ="""
      Lookup an identity from either an API pair (key and secret) or a cluster secret. Be careful
      to fill in the query string parameters for exactly one of the two options.""")
  @ApiErrors(Array(
    new ApiError(code = 400, reason = "If the identity can't be found"),
    new ApiError(code = 400,
      reason = "When other than just the pair API key/secret of cluster secret is present")
  ))
  def authenticate(
      @ApiParam(name="API key")
      apiKey: Option[String],
      @ApiParam(name="API secret")
      apiSecret: Option[String],
      @ApiParam(name="Cluster secret")
      clusterSecret: Option[String]) = Action { implicit request =>
    for {
      identity <- ((apiKey, apiSecret, clusterSecret) match {
        case (None, None, Some(secret)) => authenticateClusterSecret(secret)
        case (Some(key), Some(secret), None) => authenticateApiCredentials(key, secret)
        case _ => Message("Invalid parameters. " +
          "Fill in either apiKey/apiSecret or clusterSecret but not both").failure
      }).leftMap(message => BadRequest(Json.toJson(message)))
    } yield Ok(Json.toJson(identity))
  }

  private def authenticateClusterSecret(secret: String): Validation[Message, InfinityIdentity] =
    useExceptionAsMessage(ClusterSecret(secret))
      .flatMap(authenticator.authenticateClusterSecret)

  private def authenticateApiCredentials(
      apiKey: String, apiSecret: String): Validation[Message, InfinityIdentity] =
    useExceptionAsMessage(ApiCredentials(apiKey, apiSecret))
      .flatMap(authenticator.authenticateApiCredentials)

  private def useExceptionAsMessage[T](block: => T): Validation[Message, T] =
    Validation.fromTryCatch(block).leftMap(ex => Message(ex.getMessage))
}
