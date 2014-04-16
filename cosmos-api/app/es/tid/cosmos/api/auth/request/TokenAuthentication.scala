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

package es.tid.cosmos.api.auth.request

import scala.concurrent.Await
import scala.concurrent.duration._
import scala.util.{Failure, Success}
import scalaz.{Failure => _, Success => _, _}

import play.Logger
import play.api.mvc.RequestHeader

import es.tid.cosmos.api.auth.oauth2.OAuthProvider
import es.tid.cosmos.api.profile.UserId
import es.tid.cosmos.api.profile.dao.ProfileDataStore

private[request] class TokenAuthentication(
     oauth: OAuthProvider,
     store: ProfileDataStore,
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
    store.withTransaction { implicit c =>
      store.profile.lookupByUserId(userId)
    }.toSuccess(InvalidAuthCredentials)
}

private object TokenAuthentication {
  val DefaultTimeout = 1.second
}
