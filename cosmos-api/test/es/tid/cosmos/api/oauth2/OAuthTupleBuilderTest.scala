/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.api.oauth2

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
