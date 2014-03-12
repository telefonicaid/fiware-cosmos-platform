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

package es.tid.cosmos.api.controllers.storage

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json

class InfinityIdentityTest extends FlatSpec with MustMatchers {

  val auth = InfinityIdentity(
    user = "jsmith",
    group = "analytics",
    accessMask = AccessMask("777"),
    origins = AnyHost
  )
  val authJson = Json.obj(
    "user" -> "jsmith",
    "group" -> "analytics",
    "accessMask" -> "777",
    "origins" -> "anyHost"
  )

  "An authentication" must "be convertible to JSON" in {
    Json.toJson(auth) must be (authJson)
  }

  it must "have a list of valid hosts when using white lists" in {
    Json.toJson(auth.copy(origins = WhiteList(Set("10.0.0.2", "10.0.0.3")))) must
      be (authJson ++ Json.obj("origins" -> Json.arr("10.0.0.2", "10.0.0.3")))
  }
}
