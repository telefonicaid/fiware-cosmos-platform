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

import scala.util.Success

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json

import es.tid.cosmos.api.profile.UserId

class GitHubProfileTest extends FlatSpec with MustMatchers {

  val jSmithProfile = GitHubProfile(53, "jsmith", "John Smith", "jsmith@tid.es")
  val json = Json.obj(
    "id" -> 53,
    "login" -> "jsmith",
    "name" -> "John Smith",
    "email" -> "jsmith@tid.es",
    "site_admin" -> false
  )

  "A GitHub profile" must "be converted to a generic user profile" in {
    jSmithProfile.asUserProfile("realm") must equal (UserProfile(
      UserId("realm", "53"), Some("John Smith"), Some("jsmith@tid.es")))
  }

  it must "be parsed from a valid JSON" in {
    GitHubProfile.fromJson(json.toString) must equal (Success(jSmithProfile))
  }

  it must "be not parsed from invalid JSON" in {
    GitHubProfile.fromJson("{ broken JSON") must be ('failure)
  }

  it must "be not parsed when either id, name or email field is missing" in {
    for (missingField <- Seq("id", "name", "email")) {
      GitHubProfile.fromJson((json - missingField).toString) must be ('failure)
    }
  }
}
