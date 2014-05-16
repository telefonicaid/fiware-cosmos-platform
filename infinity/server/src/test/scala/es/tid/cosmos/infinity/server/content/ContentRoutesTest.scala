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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import es.tid.cosmos.infinity.server.config.ContentServerConfig
import com.typesafe.config.ConfigFactory
import es.tid.cosmos.infinity.server.hadoop.DataNode
import org.scalatest.mock.MockitoSugar
import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.urls.InfinityUrlMapper
import es.tid.cosmos.infinity.server.unfiltered.request.MockHttpRequest
import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.unfiltered.response.MockHttpResponse
import es.tid.cosmos.infinity.server.unfiltered.MockRequestWithResponder
import javax.servlet.http.{HttpServletRequest, HttpServletResponse}
import scala.util.Success
import unfiltered.response.{Unauthorized, Ok}

class ContentRoutesTest extends FlatSpec with MustMatchers {
  "Content routes" must "correctly route an authorized and supported GET request" in new Fixture {
    routes.intent.apply(requestResponder) must be (Success(()))
    baseResponse._status must be (Ok.code)
  }

  it must "return 401 Unauthorized on missing authorization header GET request" in new Fixture {
    routes.intent.apply(requestResponder) must be (Success(()))
    baseResponse._status must be (Unauthorized.code)
  }

  it must "return 401 Unauthorized on missing authorization header POST request" in new Fixture {
    val responder = requestResponder.copy(request = baseRequest.copy(method = "POST"))
    routes.intent.apply(responder) must be (Success(()))
    baseResponse._status must be (Unauthorized.code)
  }

  it must "return 401 Unauthorized on missing authorization header PUT request" in new Fixture {
    val responder = requestResponder.copy(request = baseRequest.copy(method = "PUT"))
    routes.intent.apply(responder) must be (Success(()))
    baseResponse._status must be (Unauthorized.code)
  }

  trait Fixture extends MockitoSugar {
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
    val someUri = "/infinityfs/v1/content/some/uri"
    val somePath = Path.absolute("/some/uri")
    val baseRequest = MockHttpRequest(underlying = _request, uri = someUri)
    val baseResponse = new MockHttpResponse(_response)
    val requestResponder = new MockRequestWithResponder(baseRequest, baseResponse)
  }
}
