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

package es.tid.cosmos.api.controllers.pages

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
