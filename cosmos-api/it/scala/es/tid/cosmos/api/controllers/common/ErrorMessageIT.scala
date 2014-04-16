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

package es.tid.cosmos.api.controllers.common

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.Mode
import play.api.libs.json.Json
import play.api.test.{FakeApplication, WithApplication}

import es.tid.cosmos.api.controllers.common.ErrorMessage.ErrorMessageWrites

class ErrorMessageIT extends FlatSpec with MustMatchers {
  val message = ErrorMessage(error = "Something failed")
  val messageWithException = ErrorMessage(
    error = "Something failed",
    exception = new RuntimeException("Missing foo")
  )

  "An error message" must "be serializable to JSON" in {
    Json.toJson(message) must be (Json.obj("error" -> "Something failed"))
  }

  it must "show exception details when not in production mode" in new WithApplication {
    val json = Json.toJson(messageWithException)
    (json \ "error").as[String] must be ("Something failed: Missing foo")
    (json \ "exception").as[String] must be ("java.lang.RuntimeException")
    (json \ "stackTrace").asOpt[List[String]] must be ('defined)
  }

  class FakeProdApplication extends FakeApplication {
    override val mode = Mode.Prod
  }

  it must "hide exception details when in production mode" in
    new WithApplication(new FakeProdApplication()) {
      val json = Json.toJson(messageWithException)
      (json \ "exception").asOpt[String] must not be ('defined)
      (json \ "stackTrace").asOpt[List[String]] must not be ('defined)
    }
}
