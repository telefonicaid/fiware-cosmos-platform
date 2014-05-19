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

import java.io.StringReader
import javax.servlet.http.{HttpServletResponse, HttpServletRequest}
import scala.concurrent.Future
import scala.concurrent.duration._
import scala.language.postfixOps
import scala.util.Success

import org.mockito.BDDMockito._
import org.mockito.Matchers._
import org.mockito.Mockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.ShouldMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.credentials.UserCredentials
import es.tid.cosmos.infinity.common.permissions.UserProfile
import es.tid.cosmos.infinity.server.actions.MetadataActionFixture
import es.tid.cosmos.infinity.server.authentication._
import es.tid.cosmos.infinity.server.errors.ErrorCode
import es.tid.cosmos.infinity.server.hadoop.NameNodeException
import es.tid.cosmos.infinity.server.unfiltered.MockRequestWithResponder
import es.tid.cosmos.infinity.server.unfiltered.request.MockHttpRequest
import es.tid.cosmos.infinity.server.unfiltered.response.MockHttpResponse
import es.tid.cosmos.infinity.server.urls.InfinityUrlMapper

class MetadataRoutesTest extends FlatSpec with ShouldMatchers with MockitoSugar with FutureMatchers {
  "Get file metadata" should "return appropriate error on missing authorization header" in new Fixture {
    app.intent.apply(baseResponder) should be (Success())
    baseResponse._status should be (401)
    baseResponse.body should include (ErrorCode.MissingAuthorizationHeader.code)
  }

  it should "return appropriate error on unsupported authorization header" in new Fixture {
    val request = baseRequest.copy(
      headerz = Map("Authorization" -> Seq("Digest dXNlcjpwYXNzd29yZA=="))) // user:password
    val responder = baseResponder.copy(request = request)
    app.intent.apply(responder) should be (Success())
    baseResponse._status should be (401)
    baseResponse.body should include (ErrorCode.UnsupportedAuthorizationHeader.code)
  }

  it should "return appropriate error on malformed key-secret pair" in new Fixture {
    val request = baseRequest.copy(
      headerz = Map("Authorization" -> Seq("Basic dXNlckBwYXNzd29yZA=="))) // user@password
    val responder = baseResponder.copy(request = request)
    app.intent.apply(responder) should be (Success())
    responder.response_> should runUnder(1 second)
    baseResponse._status should be (401)
    baseResponse.body should include (ErrorCode.MalformedKeySecretPair.code)
  }

  it should "return appropriate error on invalid basic hash" in new Fixture {
    val request = baseRequest.copy(
      headerz = Map("Authorization" -> Seq("Basic ,,,,,,,")))
    val responder = baseResponder.copy(request = request)
    app.intent.apply(responder) should be (Success())
    responder.response_> should runUnder(1 second)
    baseResponse._status should be (401)
    baseResponse.body should include (ErrorCode.InvalidBasicHash.code)
  }

  it should "return 401 on unauthenticated credentials" in new AuthenticationFailure {
    app.intent.apply(responder) should be (Success())
    responder.response_> should runUnder(1 second)
    baseResponse._status should equal(401)
  }

  it should "return 404 on non-existent files" in new Authenticated {
    doReturn(Future.failed(NameNodeException.NoSuchPath(Path.absolute("/"))))
      .when(nameNode).pathMetadata(any())
    app.intent.apply(responder) should be (Success())
    responder.response_> should runUnder(1 second)
    baseResponse._status should equal(404)
  }

  it should "return 409 when the path already exists" in new Authenticated {
    doReturn(Future.failed(NameNodeException.PathAlreadyExists(Path.absolute("/"))))
      .when(nameNode).pathMetadata(any())
    app.intent.apply(responder) should be (Success())
    responder.response_> should runUnder(1 second)
    baseResponse._status should equal(409)
  }

  it should "return 400 when the body is invalid" in new Authenticated {
    override lazy val request =  baseRequest.copy(headerz = Map(authHeader), method = "POST")
    app.intent.apply(responder) should be (Success())
    responder.response_> should runUnder(1 second)
    baseResponse._status should equal(400)
  }

  it should "return 422 when the parent is not a directory" in new Authenticated {
    doReturn(Future.failed(NameNodeException.ParentNotDirectory(Path.absolute("/"))))
      .when(nameNode).createFile(any(), any(), any(), any(), any(), any())
    override lazy val request =  baseRequest.copy(
      headerz = Map(authHeader),
      method = "POST",
      reader = new StringReader(
        """
          |{
          |  "action": "mkfile",
          |  "name": "somefile.txt",
          |  "permissions": "777",
          |  "replication": 3
          |}
        """.stripMargin))
    app.intent.apply(responder) should be (Success())
    responder.response_> should runUnder(1 second)
    baseResponse._status should equal(422)
  }

  it should "return 500 on IOErrors" in new Authenticated {
    doReturn(Future.failed(NameNodeException.IOError()))
      .when(nameNode).pathMetadata(any())
    app.intent.apply(responder) should be (Success())
    responder.response_> should runUnder(1 second)
    baseResponse._status should equal(500)
  }

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
    val baseResponder = new MockRequestWithResponder(baseRequest, baseResponse)
  }

  trait Authenticated extends Fixture {
    val authHeader = "Authorization" -> Seq("Basic YXBpLWtleTphcGktc2VjcmV0")
    val credentials = UserCredentials("api-key", "api-secret")
    val profile = UserProfile("Tyrion", groups = Seq("Lannister"))

    lazy val request = baseRequest.copy(headerz = Map(authHeader))
    lazy val responder = baseResponder.copy(request = this.request)

    given(authService.authenticate(credentials)).willReturn(Future.successful(profile))
  }

  trait AuthenticationFailure extends Fixture {
    val authHeader = "Authorization" -> Seq("Basic YXBpLWtleTphcGktc2VjcmV0")
    val credentials = UserCredentials("api-key", "api-secret")

    val request = baseRequest.copy(headerz = Map(authHeader))
    val responder = baseResponder.copy(request = this.request)

    given(authService.authenticate(credentials)).willReturn(
      Future.failed(new AuthenticationException("failed")))
  }
}
