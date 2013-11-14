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
