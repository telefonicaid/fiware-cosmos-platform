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

package es.tid.cosmos.api.auth.oauth2

import com.ning.http.client.{Response, RequestBuilder}
import dispatch.{StatusCode, as}
import org.mockito.BDDMockito.given
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

class OAuthTupleBuilderTest extends FlatSpec with MustMatchers with MockitoSugar {
  trait TupleBuilder {
    val builder = new OAuthTupleBuilder(new RequestBuilder())
    val (_, handler) = builder OAuthOK as.String
    val response = mock[Response]
  }

  "An OAuthOK handler" must "pass the response when status 200" in new TupleBuilder {
    given(response.getStatusCode).willReturn(200)
    given(response.getResponseBody).willReturn("body")
    handler.onCompleted(response) must be ("body")
  }

  it must "throw OAuthException for a known error code and status 400" in new TupleBuilder {
    given(response.getStatusCode).willReturn(400)
    given(response.getResponseBody).willReturn("error=invalid_grant")
    evaluating {
      handler.onCompleted(response)
    } must produce [OAuthException]
  }

  it must "throw StatusCode exception for unknown error codes" in new TupleBuilder {
    given(response.getStatusCode).willReturn(400)
    given(response.getResponseBody).willReturn("error=unknown")
    evaluating {
      handler.onCompleted(response)
    } must produce [StatusCode]
  }

  it must "throw StatusCode on other status codes" in new TupleBuilder {
    given(response.getStatusCode).willReturn(500)
    evaluating {
      handler.onCompleted(response)
    } must produce [StatusCode]
  }
}
