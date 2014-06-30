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
