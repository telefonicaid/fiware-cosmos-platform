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

import java.net.InetAddress
import scalaz.{Success, Validation}

import com.twitter.finagle.http.{Request, RequestBuilder}
import org.scalatest.{FlatSpec, Inside}
import org.scalatest.matchers.{MatchResult, Matcher, MustMatchers}

import es.tid.cosmos.infinity.common.credentials.{ClusterCredentials, Credentials, UserCredentials}
import es.tid.cosmos.infinity.server.errors.RequestParsingException

class HttpCredentialsValidatorTest extends FlatSpec with MustMatchers with Inside {

  val from = InetAddress.getLocalHost

  "HTTP credentials validator" must "fail to extract credentials on missing Authorization header" in {
    val req = Request(RequestBuilder()
      .url("http://example.com/")
      .buildGet())
    HttpCredentialsValidator(from, req) must failWith[RequestParsingException.MissingAuthorizationHeader]
  }

  it must "fail to extract credentials on unsupported Authorization header" in {
    val auth = "Digest YXBpLWtleTphcGktc2VjcmV0" // "api-key:api-secret"
    val req = Request(RequestBuilder()
      .url("http://example.com/")
      .addHeader("Authorization", auth)
      .buildGet())
    HttpCredentialsValidator(from, req) must failWith[RequestParsingException.UnsupportedAuthorizationHeader]
  }

  it must "extract user credentials" in {
    val req = Request(RequestBuilder()
      .url("http://example.com/")
      .addHeader("Authorization", "Basic YXBpLWtleTphcGktc2VjcmV0") // "api-key:api-secret"
      .buildGet())
    HttpCredentialsValidator(from, req) must be (Success(UserCredentials("api-key", "api-secret")))
  }

  it must "fail to extract user credentials from unexpected basic pair" in {
    val req = Request(RequestBuilder()
      .url("http://example.com/")
      .addHeader("Authorization", "Basic YXBpLWtleUBhcGktc2VjcmV0") // "api-key@api-secret"
      .buildGet())
    HttpCredentialsValidator(from, req) must failWith[RequestParsingException.MalformedKeySecretPair]
  }

  it must "fail to extract user credentials from invalid hash" in {
    val req = Request(RequestBuilder()
      .url("http://example.com/")
      .addHeader("Authorization", "Basic @@@@@@@@@")
      .buildGet())
    HttpCredentialsValidator(from, req) must failWith[RequestParsingException.InvalidBasicHash]
  }

  it must "extract cluster credentials" in {
    val req = Request(RequestBuilder()
      .url("http://example.com/")
      .addHeader("Authorization", "Bearer cluster-secret")
      .buildGet())
    HttpCredentialsValidator(from, req) must
      be (Success(ClusterCredentials(from, "cluster-secret")))
  }

  def failWith[T : Manifest] = new Matcher[Validation[RequestParsingException, Credentials]] {

    override def apply(left: Validation[RequestParsingException, Credentials]): MatchResult = {
      val expectedError = implicitly[Manifest[T]].runtimeClass
      left.fold(
        error => MatchResult(
          matches = error.getClass == expectedError,
          failureMessage = s"failed with error ${error.getClass} rather than $expectedError",
          negatedFailureMessage = s"failed with error $expectedError}"),
        success => MatchResult(
          matches = false,
          failureMessage = s"result is not error $expectedError",
          negatedFailureMessage = s"result is $expectedError")
      )
    }
  }
}
