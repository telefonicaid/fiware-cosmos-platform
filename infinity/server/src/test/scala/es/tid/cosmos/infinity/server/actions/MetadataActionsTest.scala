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
package es.tid.cosmos.infinity.server.actions

import com.twitter.finatra.FinatraServer
import com.twitter.finatra.test.SpecHelper
import org.scalatest.FlatSpec
import org.scalatest.matchers.ShouldMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.finatra._

class MetadataActionsTest extends FlatSpec with ShouldMatchers with MockitoSugar {

  "Get file metadata" should "return appropriate error on missing authorization header" in new Fixture {
    get("/infinityfs/v1/metadata/some/file.txt")
    response.code should be (401)
    response.body should include (MissingAuthorizationHeader.code)
  }

  it should "return appropriate error on unsupported authorization header" in new Fixture {
    get("/infinityfs/v1/metadata/some/file.txt", headers = Map(
      "Authorization" -> "Digest dXNlcjpwYXNzd29yZA==" // user:password
    ))
    response.code should be (401)
    response.body should include (UnsupportedAuthorizationHeader.code)
  }

  it should "return appropriate error on malformed key-secret pair" in new Fixture {
    get("/infinityfs/v1/metadata/some/file.txt", headers = Map(
      "Authorization" -> "Basic dXNlckBwYXNzd29yZA==" // user@password
    ))
    response.code should be (401)
    response.body should include (MalformedKeySecretPair.code)
  }

  it should "return appropriate error on invalid basic hash" in new Fixture {
    get("/infinityfs/v1/metadata/some/file.txt", headers = Map(
      "Authorization" -> "Basic ,,,,,,,"
    ))
    response.code should be (401)
    response.body should include (InvalidBasicHash.code)
  }


  it should "respond 200" in new Fixture {
    get("/infinityfs/v1/metadata/some/file.txt", headers = Map(
      "Authorization" -> "Basic dXNlcjpwYXNzd29yZA=="
    ))
    response.body should equal ("metadata of /some/file.txt")
    response.code should equal (200)
  }

  trait Fixture extends SpecHelper {
    override val server = new FinatraServer
    val authService = mock[AuthenticationService]
    val app = new MetadataActions(authService)
    server.register(app)
  }
}
