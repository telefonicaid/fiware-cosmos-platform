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
