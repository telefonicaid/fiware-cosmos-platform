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

package es.tid.cosmos.api.controllers.storage

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json
import java.net.URI

class InfinityConnectionTest extends FlatSpec with MustMatchers {

  "Infinity connection details" must "require locations with 'infinity' scheme" in {
    evaluating {
      InfinityConnection(location = new URI("ftp://host:8080/path"), user = "user")
    } must produce [IllegalArgumentException]
  }

  it must "be serializable to JSON" in {
    val connection = InfinityConnection(location = new URI("infinity://localhost/"), user = "jsmith")
    Json.toJson(connection) must be (Json.obj(
      "location" -> "infinity://localhost/",
      "user" -> "jsmith"
    ))
  }
}
