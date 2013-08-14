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

package es.tid.cosmos.api.oauth2

import scala.util.Try

import play.api.libs.json.Json

case class GitHubProfile(id: Int, login: String, name: String, email: String) {
  def asUserProfile: UserProfile = UserProfile(id.toString, Some(name), Some(email))
}

object GitHubProfile {
  def fromJson(jsonProfile: String): Try[GitHubProfile] = Try {
    val json = Json.parse(jsonProfile)
    GitHubProfile(
      (json \ "id").as[Int],
      (json \ "login").as[String],
      (json \ "name").as[String],
      (json \ "email").as[String]
    )
  }
}
