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

package es.tid.cosmos.api.controllers.pages

import scala.language.implicitConversions

import play.api.mvc.Session

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.profile.UserId

/**
 * Enrich via implicit conversion the default Play! session object.
 * No other class have knowledge of the keys stored on the session.
 *
 * @constructor Wrap a session
 * @param s Wrapped session
 */
class CosmosSession(val s: Session) {
  def isAuthenticated: Boolean = userProfile.isDefined
  def token: Option[String] = s.get("token")
  def setToken(token: String): Session = s + ("token", token)
  def userId: Option[UserId] = for {
    realm <- s.get("authRealm")
    id <- s.get("authId")
  } yield UserId(realm, id)
  def userProfile: Option[OAuthUserProfile] = userId.map(id => OAuthUserProfile(
    id=id,
    name=s.get("name"),
    email=s.get("email")
  ))
  def setUserProfile(profile: OAuthUserProfile): Session =
    Seq("name" -> profile.name, "email" -> profile.email)
      .foldLeft(s + ("authRealm", profile.id.realm) + ("authId", profile.id.id))(
        (s, tuple) => tuple match {
          case (key, Some(value)) => s + (key, value)
          case _ => s
        })
}

object CosmosSession {
  implicit def asCosmosSession(s: Session): CosmosSession = new CosmosSession(s)
}
