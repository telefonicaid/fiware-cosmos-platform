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

package es.tid.cosmos.api.auth.request

import scalaz._

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.mvc.RequestHeader
import play.api.test.FakeRequest

import es.tid.cosmos.api.profile.{ApiCredentials, CosmosProfile}
import es.tid.cosmos.api.profile.UserState._

class ChainedAuthenticationTest extends FlatSpec with MustMatchers {

  import Scalaz._

  class TestAuthentication(keyword: String, error: AuthError)
    extends RequestAuthentication {

    val profile = CosmosProfile(
      id = 10,
      state = Enabled,
      handle = s"user$keyword",
      email = "user@host",
      apiCredentials = ApiCredentials.random(),
      keys = Seq.empty
    )

    override def authenticateRequest(request: RequestHeader): AuthResult =
      if (request.path.contains(keyword)) profile.success
      else error.fail
  }

  val chain = new ChainedAuthentication(Seq(
    new TestAuthentication("a", InvalidProfileState(Creating)),
    new TestAuthentication("b", InvalidProfileState(Disabled)),
    new TestAuthentication("c", InvalidProfileState(Deleting))
  ))

  "Chained authentication" must "return the first successful authentication in the chain" in {
    chain.authenticateRequest(FakeRequest("GET", "abc")).map(_.handle) must be ("usera".success)
    chain.authenticateRequest(FakeRequest("GET", "-bc")).map(_.handle) must be ("userb".success)
    chain.authenticateRequest(FakeRequest("GET", "--c")).map(_.handle) must be ("userc".success)
  }

  it must "return the first error when the whole chain fail" in {
    chain.authenticateRequest(FakeRequest("GET", "")).map(_.handle) must
      be (InvalidProfileState(Creating).failure)
  }
}
