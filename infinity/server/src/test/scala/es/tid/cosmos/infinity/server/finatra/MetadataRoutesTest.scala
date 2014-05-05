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
package es.tid.cosmos.infinity.server.finatra

import scala.concurrent.Future

import com.twitter.finatra.FinatraServer
import com.twitter.finatra.test.SpecHelper
import com.typesafe.config.ConfigFactory
import org.apache.hadoop.hdfs.server.protocol.NamenodeProtocols
import org.mockito.Matchers._
import org.mockito.Mockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.ShouldMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.common.UserProfile
import es.tid.cosmos.infinity.server.authentication._
import es.tid.cosmos.infinity.server.config.InfinityConfig

class MetadataRoutesTest extends FlatSpec with ShouldMatchers with MockitoSugar {

  "Get file metadata" should "return appropriate error on missing authorization header" in new Fixture {
    get("/infinityfs/v1/metadata/some/file.txt")
    response.code should be (401)
    response.body should include (ErrorCode.MissingAuthorizationHeader.code)
  }

  it should "return appropriate error on unsupported authorization header" in new Fixture {
    get("/infinityfs/v1/metadata/some/file.txt", headers = Map(
      "Authorization" -> "Digest dXNlcjpwYXNzd29yZA==" // user:password
    ))
    response.code should be (401)
    response.body should include (ErrorCode.UnsupportedAuthorizationHeader.code)
  }

  it should "return appropriate error on malformed key-secret pair" in new Fixture {
    get("/infinityfs/v1/metadata/some/file.txt", headers = Map(
      "Authorization" -> "Basic dXNlckBwYXNzd29yZA==" // user@password
    ))
    response.code should be (401)
    response.body should include (ErrorCode.MalformedKeySecretPair.code)
  }

  it should "return appropriate error on invalid basic hash" in new Fixture {
    get("/infinityfs/v1/metadata/some/file.txt", headers = Map(
      "Authorization" -> "Basic ,,,,,,,"
    ))
    response.code should be (401)
    response.body should include (ErrorCode.InvalidBasicHash.code)
  }

  it should "return 401 on unauthenticated credentials" in new Fixture {
    givenFailedAuthentication {
      get("/infinityfs/v1/metadata/some/file.txt", headers = Map(
        "Authorization" -> "Basic dXNlcjpwYXNzd29yZA=="
      ))
      response.code should equal(401)
    }
  }

  trait Fixture extends SpecHelper {
    val config = new InfinityConfig(ConfigFactory.load())
    val nameNode = mock[NamenodeProtocols]
    val urlMapper = new FinatraUrlMapper(config)
    val authService = mock[AuthenticationService]
    override val server = new FinatraServer
    val app = new MetadataRoutes(config, authService, nameNode, urlMapper)
    server.register(app)

    def givenSuccessAuthentication(action: => Unit) {
      when(authService.authenticate(anyObject())).thenReturn(Future.successful(UserProfile(
        username = "gandalf",
        group = "istari"
      )))
      action
    }

    def givenFailedAuthentication(action: => Unit) {
      when(authService.authenticate(anyObject()))
        .thenReturn(Future.failed(new AuthenticationException("failed")))
      action
    }
  }

}
