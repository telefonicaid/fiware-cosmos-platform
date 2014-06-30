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

package es.tid.cosmos.api.controllers.infinity

import scala.concurrent.Future
import scalaz.Failure

import com.typesafe.config.ConfigFactory
import org.scalatest.{FlatSpec, Inside}
import org.scalatest.matchers.MustMatchers
import play.api.mvc._
import play.api.test.FakeRequest
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.common.ActionValidation
import es.tid.cosmos.common.BearerToken

class InfinityRequestAuthenticationTest extends FlatSpec with MustMatchers with Inside {

  val secret = "s3cr3t"
  val authentication = new InfinityRequestAuthentication(
    ConfigFactory.parseString(s"""infinity.secret = "$secret" """)
  )

  "Infinity request authentication" must "throw if Infinity secret is not configured" in {
    evaluating {
      new InfinityRequestAuthentication(ConfigFactory.empty())
    } must produce [IllegalStateException]
  }

  it must "reject requests without an authentication header" in {
    val res = failureResponse(authentication.requireAuthorized(FakeRequest(GET, "/some/path")))
    status(res) must be (UNAUTHORIZED)
    contentAsString(res) must include ("Missing authorization header")
  }

  it must "reject request with wrong credentials" in {
    val request = FakeRequest(GET, "/some/path")
      .withHeaders("Authorization" -> BearerToken("wrong_credentials"))
    val res = failureResponse(authentication.requireAuthorized(request))
    status(res) must be (UNAUTHORIZED)
    contentAsString(res) must include ("Invalid credentials")
  }

  it must "accept request with Infinity credentials" in {
    val request = FakeRequest(GET, "/some/path").withHeaders("Authorization" -> BearerToken(secret))
    authentication.requireAuthorized(request) must be ('success)
  }

  private def failureResponse(response: ActionValidation[Unit]): Future[SimpleResult] =
    response match {
      case Failure(simpleResult) => Future.successful(simpleResult)
      case _ => fail(s"Validation $response has not failed")
    }
}
