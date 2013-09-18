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

import es.tid.cosmos.api.controllers.pages.NamedKey

class UserProfileTest extends FlatSpec with MustMatchers {

  "A user profile" must "be serializable to JSON" in {
    val profile = UserProfile("user1", List(NamedKey("key", "sig")))
    Json.toJson(profile) must equal (Json.obj(
      "handle" -> "user1",
      "keys" -> Json.arr(
        Json.obj("name" -> "key", "signature" -> "sig")
      )
    ))
  }

  it must "sort keys by name" in {
    val profile = UserProfile("user1", List(
      NamedKey("key3", "sig_a"),
      NamedKey("key1", "sig_b"),
      NamedKey("key2", "sig_c")
    ))
    (Json.toJson(profile) \\ "name").map(_.as[String]) must equal (Seq("key1", "key2", "key3"))
  }
}
