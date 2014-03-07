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

import scala.concurrent.Await
import scala.concurrent.duration._
import scala.util.{Failure, Success}
import scalaz.{Failure => _, Success => _, _}

import play.Logger
import play.api.mvc.RequestHeader

import es.tid.cosmos.api.auth.oauth2.OAuthProvider
import es.tid.cosmos.api.profile.{CosmosDao, UserId}

private[request] class TokenAuthentication(
     oauth: OAuthProvider,
     dao: CosmosDao,
     timeout: FiniteDuration = TokenAuthentication.DefaultTimeout)
  extends RequestAuthentication {

  import Scalaz._

  override def authenticateRequest(request: RequestHeader): AuthResult = for {
    token <- requireTokenHeader(request)
    userId <- requireValidToken(token)
    profile <- requireExistingProfile(userId)
  } yield profile

  private def requireTokenHeader(request: RequestHeader): Validation[AuthError, String] =
    request.headers.get("X-Auth-Token").toSuccess(MissingAuthentication)

  private def requireValidToken(token: String): Validation[AuthError, UserId] = {
    val profileRequest = oauth.requestUserProfile(token)
    Await.ready(profileRequest, timeout)
    profileRequest.value.get match {
      case Failure(ex) =>
        Logger.error(s"Cannot retrieve user profile from token '$token'", ex)
        CannotRetrieveProfile(ex).failure
      case Success(oauthProfile) => oauthProfile.id.success
    }
  }

  private def requireExistingProfile(userId: UserId): AuthResult =
    dao.withTransaction { implicit c =>
      dao.profile.lookupByUserId(userId)
    }.toSuccess(InvalidAuthCredentials)
}

private object TokenAuthentication {
  val DefaultTimeout = 1.second
}
