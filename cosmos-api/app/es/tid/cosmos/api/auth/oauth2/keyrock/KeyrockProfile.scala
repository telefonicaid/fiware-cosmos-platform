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

package es.tid.cosmos.api.auth.oauth2.keyrock

import play.api.libs.json.Json

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.profile.UserId

private[keyrock] case class KeyrockProfile(
    id: Int,
    actorId: Int,
    nickName: String,
    displayName: String,
    email: String) {

  def asUserProfile(realm: String): OAuthUserProfile = OAuthUserProfile(
    id = UserId(realm, actorId.toString),
    name = Some(displayName),
    email = Some(email)
  )
}

private[keyrock] object KeyrockProfile {
  implicit val keyrockProfileReads = Json.reads[KeyrockProfile]
}
