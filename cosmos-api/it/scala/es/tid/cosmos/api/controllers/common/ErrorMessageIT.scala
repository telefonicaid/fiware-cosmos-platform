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

package es.tid.cosmos.api.controllers.common

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.Mode
import play.api.libs.json.{JsArray, Json}
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
    (json \ "stack_trace").getClass must be (classOf[JsArray])
  }

  class FakeProdApplication extends FakeApplication {
    override val mode = Mode.Prod
  }

  it must "hide exception details when in production mode" in
    new WithApplication(new FakeProdApplication()) {
      val json = Json.toJson(messageWithException)
      (json \ "exception").asOpt[String] must not be ('defined)
      (json \ "stack_trace").asOpt[String] must not be ('defined)
    }
}
