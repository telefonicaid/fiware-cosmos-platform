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
