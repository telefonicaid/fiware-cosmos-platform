/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.server.routes

import javax.servlet.http.{HttpServletRequest, HttpServletResponse}
import scala.concurrent.Future
import scala.concurrent.duration._
import scala.language.postfixOps
import scala.util.Success

import org.mockito.BDDMockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import unfiltered.filter.async.Plan.Intent
import unfiltered.response.{Unauthorized, NotFound}

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.credentials.UserCredentials
import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.permissions.UserProfile
import es.tid.cosmos.infinity.server.authentication.{AuthenticationException, AuthenticationService}
import es.tid.cosmos.infinity.server.errors.ErrorCode
import es.tid.cosmos.infinity.server.unfiltered.MockRequestWithResponder
import es.tid.cosmos.infinity.server.unfiltered.request.MockHttpRequest
import es.tid.cosmos.infinity.server.unfiltered.response.MockHttpResponse

trait RoutesBehavior[HadoopApi] extends MustMatchers with FutureMatchers { this: FlatSpec =>

  /** Function allowing to enrich the test's default GET request  */
  type RequestFunction = MockHttpRequest[HttpServletRequest] => MockHttpRequest[HttpServletRequest]
  /** Type for side effect functions that allow adding mock behavior to the hadoop API */
  type AddHadoopBehavior = HadoopApi => Unit

  def newRoutes[Api]: Routes

  def supportsAuthorization(requestTransformation: RequestFunction = identity): Unit = {

    it must "return appropriate error on missing authorization header" in
      new Fixture(requestTransformation) {
        routes.intent.apply(baseResponder) must be (Success())
        baseResponse._status must be (Unauthorized.code)
        baseResponse.body must include (ErrorCode.MissingAuthorizationHeader.code)
      }

    it must "return appropriate error on unsupported authorization header" in
      new Fixture(requestTransformation) {
        val request = baseRequest.copy(
          headerz = Map("Authorization" -> Seq("Digest dXNlcjpwYXNzd29yZA=="))) // user:password
        val responder = baseResponder.copy(request = request)
        routes.intent.apply(responder) must be (Success())
        baseResponse._status must be (Unauthorized.code)
        baseResponse.body must include (ErrorCode.UnsupportedAuthorizationHeader.code)
      }

    it must "return appropriate error on malformed key-secret pair" in
      new Fixture(requestTransformation) {
        val request = baseRequest.copy(
          headerz = Map("Authorization" -> Seq("Basic dXNlckBwYXNzd29yZA=="))) // user@password
        val responder = baseResponder.copy(request = request)
        routes.intent.apply(responder) must be (Success())
        responder.response_> must runUnder(1 second)
        baseResponse._status must be (Unauthorized.code)
        baseResponse.body must include (ErrorCode.MalformedKeySecretPair.code)
      }

    it must "return appropriate error on invalid basic hash" in
      new Fixture(requestTransformation) {
        val request = baseRequest.copy(
          headerz = Map("Authorization" -> Seq("Basic ,,,,,,,")))
        val responder = baseResponder.copy(request = request)
        routes.intent.apply(responder) must be (Success())
        responder.response_> must runUnder(1 second)
        baseResponse._status must be (Unauthorized.code)
        baseResponse.body must include (ErrorCode.InvalidBasicHash.code)
      }

    it must "return 401 on unauthenticated credentials" in
      new AuthenticationFailure(requestTransformation) {
        routes.intent.apply(responder) must be (Success())
        responder.response_> must runUnder(1 second)
        baseResponse._status must equal(Unauthorized.code)
      }
  }

  def canHandleNotFound(
      requestTransformation: RequestFunction = identity,
      hadoopBehavior: AddHadoopBehavior): Unit = {

    it must "return 404 NotFound when the target file/directory does not exist" in
      new Authenticated(requestTransformation) {
        hadoopBehavior(routes.hadoopApi)
        routes.intent.apply(responder) must be (Success())
        responder.response_> must runUnder(1 second)
        baseResponse._status must equal(NotFound.code)
      }
  }

  trait Routes extends MockitoSugar {
    val intent: Intent
    val someUri: String
    val somePath: Path
    val authService: AuthenticationService
    val hadoopApi: HadoopApi
  }

  abstract class Fixture(requestTransformation: RequestFunction) extends MockitoSugar {
    val routes = newRoutes
    val _request = mock[HttpServletRequest]("servletRequest")
    val _response = mock[HttpServletResponse]("servletResponse")
    val baseRequest = requestTransformation(
      MockHttpRequest(underlying = _request, uri = routes.someUri, method = "GET")
    )
    val baseResponse = new MockHttpResponse(_response)
    val baseResponder = new MockRequestWithResponder(baseRequest, baseResponse)
  }

  abstract class Authenticated(requestTransformation: RequestFunction)
      extends Fixture(requestTransformation) {
    val authHeader = "Authorization" -> Seq("Basic YXBpLWtleTphcGktc2VjcmV0")
    val credentials = UserCredentials("api-key", "api-secret")
    val profile = UserProfile("Tyrion", groups = Seq("Lannister"))

    lazy val request = baseRequest.copy(headerz = Map(authHeader))
    lazy val responder = baseResponder.copy(request = this.request)

    given(routes.authService.authenticate(credentials)).willReturn(Future.successful(profile))
  }

  abstract class AuthenticationFailure(test: RequestFunction) extends Fixture(test) {
    val authHeader = "Authorization" -> Seq("Basic YXBpLWtleTphcGktc2VjcmV0")
    val credentials = UserCredentials("api-key", "api-secret")

    val request = baseRequest.copy(headerz = Map(authHeader))
    val responder = baseResponder.copy(request = this.request)

    given(routes.authService.authenticate(credentials)).willReturn(
      Future.failed(new AuthenticationException("failed")))
  }
}
