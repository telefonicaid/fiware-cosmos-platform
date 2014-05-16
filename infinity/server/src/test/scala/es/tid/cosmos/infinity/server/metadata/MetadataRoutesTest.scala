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
package es.tid.cosmos.infinity.server.metadata

import javax.servlet.http.{HttpServletResponse, HttpServletRequest}
import scala.concurrent.Future
import scala.util.Success

import org.mockito.Matchers._
import org.mockito.Mockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.ShouldMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.permissions.UserProfile
import es.tid.cosmos.infinity.server.actions.MetadataActionFixture
import es.tid.cosmos.infinity.server.authentication._
import es.tid.cosmos.infinity.server.errors.ErrorCode
import es.tid.cosmos.infinity.server.hadoop.NameNodeException
import es.tid.cosmos.infinity.server.unfiltered.MockRequestWithResponder
import es.tid.cosmos.infinity.server.unfiltered.request.MockHttpRequest
import es.tid.cosmos.infinity.server.unfiltered.response.MockHttpResponse
import es.tid.cosmos.infinity.server.urls.InfinityUrlMapper

class MetadataRoutesTest extends FlatSpec with ShouldMatchers with MockitoSugar {
  "Get file metadata" should "return appropriate error on missing authorization header" in new Fixture {
    app.intent.apply(requestResponder) should be (Success())
    baseResponse._status should be (401)
    baseResponse._out.toString should include (ErrorCode.MissingAuthorizationHeader.code)
  }

  /*
  it should "return appropriate error on unsupported authorization header" in new Fixture {
    app.intent.apply(requestResponder) should be (Success())
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

  it should "return 404 on non-existent files" in new Fixture {
    givenSuccessAuthentication {
      doReturn(Future.failed(NameNodeException.NoSuchPath(Path.absolute("/"))))
        .when(nameNode).pathMetadata(any())
      get("/infinityfs/v1/metadata/some/file.txt", headers = Map(
        "Authorization" -> "Basic dXNlcjpwYXNzd29yZA=="
      ))
      response.code should equal(404)
    }
  }

  it should "return 409 when the path already exists" in new Fixture {
    givenSuccessAuthentication {
      doReturn(Future.failed(NameNodeException.PathAlreadyExists(Path.absolute("/"))))
        .when(nameNode).pathMetadata(any())
      get("/infinityfs/v1/metadata/some/file.txt", headers = Map(
        "Authorization" -> "Basic dXNlcjpwYXNzd29yZA=="
      ))
      response.code should equal(409)
    }
  }

  it should "return 400 when the body is invalid" in new Fixture {
    givenSuccessAuthentication {
      post("/infinityfs/v1/metadata/some/file.txt", headers = Map(
        "Authorization" -> "Basic dXNlcjpwYXNzd29yZA=="
      ))
      response.code should equal(400)
    }
  }

  ignore should "return 422 when the parent is not a directory" in new Fixture {
    givenSuccessAuthentication {
      doReturn(Future.failed(NameNodeException.ParentNotDirectory(Path.absolute("/"))))
        .when(nameNode).createFile(any(), any(), any(), any(), any(), any())
      post("/infinityfs/v1/metadata/some/file.txt", headers = Map(
        "Authorization" -> "Basic dXNlcjpwYXNzd29yZA=="
      ))
      // TODO: Finagle doesn't provide a way to add a body to test requests.
      // I'm leaving the test so we can reuse it once we move to unfiltered
      response.code should equal(422)
    }
  }

  it should "return 500 on IOErrors" in new Fixture {
    givenSuccessAuthentication {
      doReturn(Future.failed(NameNodeException.IOError()))
        .when(nameNode).pathMetadata(any())
      get("/infinityfs/v1/metadata/some/file.txt", headers = Map(
        "Authorization" -> "Basic dXNlcjpwYXNzd29yZA=="
      ))
      response.code should equal(500)
    }
  }*/

  trait Fixture extends MetadataActionFixture {
    override val urlMapper = new InfinityUrlMapper(config)
    val authService = mock[AuthenticationService]
    val app = new MetadataRoutes(config, authService, nameNode, urlMapper)
    val _request = mock[HttpServletRequest]("servletRequest")
    val _response = mock[HttpServletResponse]("servletResponse")
    val someUri = "/infinityfs/v1/metadata/some/file.txt"
    val somePath = Path.absolute("/some/file.txt")
    val baseRequest = MockHttpRequest(underlying = _request, uri = someUri, method = "GET")
    val baseResponse = new MockHttpResponse(_response)
    val requestResponder = new MockRequestWithResponder(baseRequest, baseResponse)

    def givenSuccessAuthentication(action: => Unit) {
      when(authService.authenticate(anyObject())).thenReturn(Future.successful(UserProfile(
        username = "gandalf",
        groups = Seq("istari")
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
