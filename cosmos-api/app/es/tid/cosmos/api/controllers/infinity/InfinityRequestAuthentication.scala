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

import scalaz.Scalaz

import com.typesafe.config.{Config, ConfigException}
import play.api.libs.json.Json
import play.api.mvc.{RequestHeader, Results}

import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.common.BearerToken

/** Validates that Infinity requests are properly authorized. */
class InfinityRequestAuthentication(config: Config) extends Results {

  import Scalaz._
  import InfinityRequestAuthentication._

  def requireAuthorized(request: RequestHeader): ActionValidation[Unit] = for {
    authHeader <- request.headers.get("Authorization").toSuccess(MissingAuthorizationResponse)
    _ <- requireInfinityCredentials(authHeader)
  } yield ()

  private def requireInfinityCredentials(authHeader: String): ActionValidation[Unit] =
    authHeader match {
      case BearerToken(`infinitySecret`) => ().success
      case _ => invalidCredentialsResponse(authHeader).failure
    }

  private val infinitySecret = try {
    config.getString("infinity.secret")
  } catch {
    case ex: ConfigException.Missing =>
      throw new IllegalStateException("Infinity secret is not configured", ex)
  }

  private def invalidCredentialsResponse(authHeader: String) =
    Unauthorized(Json.toJson(Message(s"Invalid credentials: $authHeader")))
}

private object InfinityRequestAuthentication extends Results {

  val MissingAuthorizationResponse =
    Unauthorized(Json.toJson(Message("Missing authorization header")))
      .withHeaders("WWW-Authenticate" -> """Bearer realm="Infinity"""")
}
