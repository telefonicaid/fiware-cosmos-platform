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

package es.tid.cosmos.api.controllers.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json

import es.tid.cosmos.api.profile.NamedKey

class UserProfileTest extends FlatSpec with MustMatchers {

  val email = "user1@example.com"
  val signature = s"ssh-rsa sig $email"
  val profile1 = UserProfile("user1", email, List(NamedKey("key", signature)))
  val profile1InJson = Json.obj(
    "handle" -> "user1",
    "email" -> email,
    "keys" -> Json.arr(Json.obj(
      "name" -> "key",
      "signature" -> signature
    ))
  )

  "A user profile" must "be serializable to JSON" in {
    Json.toJson(profile1) must equal (profile1InJson)
  }

  it must "sort keys by name" in {
    val profile = UserProfile("user1", "a@example.com", List(
      NamedKey("key3", "ssh-rsa sig_a a@example.com"),
      NamedKey("key1", "ssh-rsa sig_b b@example.com"),
      NamedKey("key2", "ssh-rsa sig_c c@example.com")
    ))
    (Json.toJson(profile) \\ "name").map(_.as[String]) must equal (Seq("key1", "key2", "key3"))
  }

  it must "be deserializable from JSON" in {
    Json.fromJson[UserProfile](profile1InJson).get must equal (profile1)
  }
}
