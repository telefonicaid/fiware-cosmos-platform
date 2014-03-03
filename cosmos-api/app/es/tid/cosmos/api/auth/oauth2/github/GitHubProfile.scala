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

package es.tid.cosmos.api.auth.oauth2.github

import play.api.libs.json._

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.profile.UserId

private[github] case class GitHubProfile(id: Int, login: String, name: String, email: String) {
  def asUserProfile(realm: String): OAuthUserProfile =
    OAuthUserProfile(UserId(realm, id.toString), Some(name), Some(email))
}

private[github] object GitHubProfile {

  implicit val GitHubProfileReads = Json.reads[GitHubProfile]
}
