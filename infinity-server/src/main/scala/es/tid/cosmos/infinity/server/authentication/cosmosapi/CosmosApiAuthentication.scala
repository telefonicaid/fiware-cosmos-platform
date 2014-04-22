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

package es.tid.cosmos.infinity.server.authentication.cosmosapi

import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global
import scala.util.control.NonFatal

import com.ning.http.client.RequestBuilder
import com.typesafe.config.Config
import dispatch.{Future => _, _}

import es.tid.cosmos.common.{Wrapped, BearerToken}
import es.tid.cosmos.infinity.server.authentication._

private[cosmosapi] class CosmosApiAuthentication(
    apiBase: String, infinitySecret: String, superGroup: String) extends Authentication {

  private def profileParser = new UserProfileParser(superGroup)

  override def authenticate(credentials: Credentials): Future[UserProfile] = for {
    response <- requestUserAuthentication(credentials)
    profile = parseResponse(response)
  } yield {
    requireValidOrigin(credentials, profile)
    profile
  }

  private def parseResponse(response: String): UserProfile =  try {
    profileParser.parse(response)
  } catch {
    case NonFatal(ex) => throw AuthenticationException.invalidProfile(response, ex)
  }

  private def requestUserAuthentication(credentials: Credentials) =
    Http(resource() <<? queryParameters(credentials) <:< Map(authHeader) OK as.String).recoverWith {
      case Wrapped(ex @ StatusCode(403)) =>
        Future.failed(AuthenticationException.authenticationRejected(ex))
      case NonFatal(ex) =>
        Future.failed(AuthenticationException.cannotAccessService(ex))
    }

  private val authHeader = "Authorization" -> BearerToken(infinitySecret)

  private def queryParameters(credentials: Credentials) = credentials match {
    case UserCredentials(key, secret) => Map("apiKey" -> key, "apiSecret" -> secret)
    case ClusterCredentials(_, secret) => Map("clusterSecret" -> secret)
  }

  private def resource(): RequestBuilder = url(apiBase) / "infinity" / "v1" / "auth"

  private def requireValidOrigin(credentials: Credentials, profile: UserProfile): Unit =
    credentials match {
      case ClusterCredentials(origin, _) =>
        if (!profile.accessibleFrom(origin)) {
          throw AuthenticationException.invalidOrigin(origin, profile)
        }
      case UserCredentials(_, _) =>
        // Always allowed
    }
}

private[cosmosapi] object CosmosApiAuthentication {

  /** Creates a CosmosApiAuthentication taking its parameters from the passed configuration */
  def fromConfig(config: Config): CosmosApiAuthentication = new CosmosApiAuthentication(
    apiBase = config.getString("apiBase"),
    infinitySecret = config.getString("secret"),
    superGroup = config.getString("supergroup")
  )
}
