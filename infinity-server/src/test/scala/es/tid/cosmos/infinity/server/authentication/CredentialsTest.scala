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

package es.tid.cosmos.infinity.server.authentication

import java.net.InetAddress

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import spray.http._
import spray.http.HttpHeaders.Authorization
import spray.httpx.RequestBuilding.RequestBuilder

class CredentialsTest extends FlatSpec with MustMatchers {

  val remoteAddress = InetAddress.getByName("10.0.0.1")
  def requestBuilder() = new RequestBuilder(HttpMethods.GET)
    .apply("/infinityfs/v1/metadata/some/path")

  "Credentials" must "be none when missing 'authorization' header" in {
    evaluating {
      Credentials.from(remoteAddress, requestBuilder())
    } must produce [MissingAuthorizationHeader.type]
  }

  it must "be instantiated from user authentication request" in {
    val credentials = Credentials.from(
      remoteAddress,
      request = requestBuilder()
        .withHeaders(Authorization(BasicHttpCredentials("key", "secret")))
    )
    credentials must be (UserCredentials("key", "secret"))
  }

  it must "be instantiated from cluster authentication request" in {
    val credentials = Credentials.from(
      remoteAddress,
      request = requestBuilder().withHeaders(Authorization(OAuth2BearerToken("secret")))
    )
    credentials must be (ClusterCredentials("10.0.0.1", "secret"))
  }

  it must "be none for unsupported 'authorization' headers" in {
    evaluating {
      Credentials.from(remoteAddress, requestBuilder()
        .withHeaders(Authorization(GenericHttpCredentials("unsupported", "authentication"))))
    } must produce [UnsupportedAuthorizationScheme]
  }
}
