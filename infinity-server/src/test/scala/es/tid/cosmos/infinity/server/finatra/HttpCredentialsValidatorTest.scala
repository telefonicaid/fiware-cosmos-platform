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

package es.tid.cosmos.infinity.server.finatra

import java.net.InetAddress
import scalaz.{Failure, Success}

import com.twitter.finagle.http.{Request, RequestBuilder}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.server.authentication.{ClusterCredentials, UserCredentials}
import es.tid.cosmos.infinity.server.finatra.HttpCredentialsValidator._

class HttpCredentialsValidatorTest extends FlatSpec with MustMatchers {

  val from = InetAddress.getLocalHost

  "Valid HTTP credentials" must "fail to extract credentials on missing Authorization header" in {
    val req = Request(RequestBuilder()
      .url("http://example.com/")
      .buildGet())
    HttpCredentialsValidator(from, req) must be (Failure(MissingAuthorizationHeader))
  }

  it must "fail to extract credentials on unsupported Authorization header" in {
    val auth = "Digest YXBpLWtleTphcGktc2VjcmV0" // "api-key:api-secret"
    val req = Request(RequestBuilder()
      .url("http://example.com/")
      .addHeader("Authorization", auth)
      .buildGet())
    HttpCredentialsValidator(from, req) must be (Failure(UnsupportedAuthorizationHeader(auth)))
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
    HttpCredentialsValidator(from, req) must be (Failure(MalformedKeySecretPair("api-key@api-secret")))
  }

  it must "fail to extract user credentials from invalid hash" in {
    val req = Request(RequestBuilder()
      .url("http://example.com/")
      .addHeader("Authorization", "Basic @@@@@@@@@")
      .buildGet())
    HttpCredentialsValidator(from, req) must be (Failure(InvalidBasicHash("@@@@@@@@@")))
  }

  it must "extract cluster credentials" in {
    val req = Request(RequestBuilder()
      .url("http://example.com/")
      .addHeader("Authorization", "Bearer cluster-secret")
      .buildGet())
    HttpCredentialsValidator(from, req) must be (Success(ClusterCredentials(from, "cluster-secret")))
  }
}
