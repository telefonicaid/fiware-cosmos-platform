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

package es.tid.cosmos.api.controllers.infinity

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json

class InfinityIdentityTest extends FlatSpec with MustMatchers {

  val auth = InfinityIdentity(
    user = "jsmith",
    groups = Seq("analytics"),
    accessMask = AccessMask("777")
  )
  val authJson = Json.obj(
    "user" -> "jsmith",
    "groups" -> Seq("analytics"),
    "accessMask" -> "777"
  )

  "An authentication" must "be convertible to JSON" in {
    Json.toJson(auth) must be (authJson)
  }

  it must "have a list of valid hosts when using white lists" in {
    val authWithWhiteList = auth.copy(origins = Some(Set("10.0.0.2", "10.0.0.3")))
    Json.toJson(authWithWhiteList) must be (authJson ++ Json.obj(
      "origins" -> Json.arr("10.0.0.2", "10.0.0.3")
    ))
  }
}
