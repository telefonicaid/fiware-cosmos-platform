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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.{JsSuccess, JsNull, Json}

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.profile.UserId

class GitHubProfileTest extends FlatSpec with MustMatchers {

  val profile = GitHubProfile(53, "jsmith", Some("John Smith"), Some("jsmith@tid.es"))
  val profileJson = Json.obj(
    "id" -> 53,
    "login" -> "jsmith",
    "name" -> "John Smith",
    "email" -> "jsmith@tid.es"
  )

  "A GitHub profile" must "be converted to a generic user profile" in {
    profile.asUserProfile("realm") must equal (OAuthUserProfile(
      UserId("realm", "53"), Some("John Smith"), Some("jsmith@tid.es")))
  }

  it must "be parsed from JSON" in {
    Json.fromJson[GitHubProfile](profileJson) must be (JsSuccess(profile))
  }

  it must "be parsed when fields are null" in {
    Json.fromJson[GitHubProfile](profileJson ++ Json.obj(
      "email" -> JsNull,
      "name" -> JsNull
    )) must be (JsSuccess(profile.copy(email = None, name = None)))
  }
}
