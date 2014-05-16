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

package es.tid.cosmos.infinity.server.content

import java.io.ByteArrayInputStream
import java.nio.charset.Charset
import javax.servlet.http.{HttpServletRequest, HttpServletResponse}
import scala.concurrent.Future
import scala.util.Success

import com.typesafe.config.ConfigFactory
import org.mockito.BDDMockito.given
import org.mockito.Matchers.{any, eq => the}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import unfiltered.response.{Unauthorized, Ok}

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.credentials.UserCredentials
import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.config.ContentServerConfig
import es.tid.cosmos.infinity.server.hadoop.DataNode
import es.tid.cosmos.infinity.server.unfiltered.MockRequestWithResponder
import es.tid.cosmos.infinity.server.unfiltered.request.MockHttpRequest
import es.tid.cosmos.infinity.server.unfiltered.response.MockHttpResponse
import es.tid.cosmos.infinity.server.urls.InfinityUrlMapper
import es.tid.cosmos.infinity.common.permissions.UserProfile
import es.tid.cosmos.infinity.server.util.ToClose

class ContentRoutesTest extends FlatSpec with MustMatchers {
  "Content routes" must "correctly route an authorized and supported get content request" in
    new Authorized {
      routes.intent.apply(responder) must be (Success(()))
      baseResponse._status must be (Ok.code)
      baseResponse.body must be (body)
    }

//  it must "return 404 NotFound on getting content of a file that does not exist" in new Authorized {
//
//  }

  it must "return 401 Unauthorized on missing authorization header GET request" in new Fixture {
    routes.intent.apply(baseResponder) must be (Success(()))
    baseResponse._status must be (Unauthorized.code)
  }

  it must "return 401 Unauthorized on missing authorization header POST request" in new Fixture {
    val responder = baseResponder.copy(request = baseRequest.copy(method = "POST"))
    routes.intent.apply(responder) must be (Success(()))
    baseResponse._status must be (Unauthorized.code)
  }

  it must "return 401 Unauthorized on missing authorization header PUT request" in new Fixture {
    val responder = baseResponder.copy(request = baseRequest.copy(method = "PUT"))
    routes.intent.apply(responder) must be (Success(()))
    baseResponse._status must be (Unauthorized.code)
  }

  trait Fixture extends MockitoSugar {
    val someUri = "/infinityfs/v1/content/some/uri"
    val somePath = Path.absolute("/some/uri")
    val body = "aBody"

    val config = new ContentServerConfig(ConfigFactory.load())
    val dataNode = mock[DataNode]("dataNode")
    val authService = mock[AuthenticationService]
    val urlMapper = new InfinityUrlMapper(config)
    val routes = new ContentRoutes(
      config,
      authService,
      dataNode,
      urlMapper
    )
    val _request = mock[HttpServletRequest]("servletRequest")
    val _response = mock[HttpServletResponse]("servletResponse")
    val baseRequest = MockHttpRequest(underlying = _request, uri = someUri)
    val baseResponse = new MockHttpResponse(_response)
    val baseResponder = new MockRequestWithResponder(baseRequest, baseResponse)

    val inputStream = new ByteArrayInputStream(body.getBytes)
  }

  trait Authorized extends Fixture {
    val authHeader = "Authorization" -> Seq("Basic YXBpLWtleTphcGktc2VjcmV0")
    val credentials = UserCredentials("api-key", "api-secret")
    val profile = UserProfile("Tyrion", groups = Seq.empty)

    val request = baseRequest.copy(headerz = Map(authHeader))
    val responder = baseResponder.copy(request = this.request)

    given(authService.authenticate(credentials)).willReturn(Future.successful(profile))
    given(dataNode.open(the(somePath), any[Option[Long]], any[Option[Long]]))
      .willReturn(Future.successful(ToClose(inputStream)))
  }
}
