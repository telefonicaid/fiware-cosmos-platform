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

package es.tid.cosmos.infinity.server.authentication.cosmosapi

import scala.util.Success

import com.twitter.finatra.ResponseBuilder
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class ResponseObjectTest extends FlatSpec with MustMatchers {

  "Response object" must "be extracted from HTTP response" in {
    ResponseObject.extractFrom(ResponseBuilder(200,
      """{
        | "user": "gandalf",
        | "group": "istari",
        | "accessMask": "752",
        | "origins": "anyHost"
        |}
      """.stripMargin
    )) must be (Success(ResponseObject(
      user = "gandalf",
      group = "istari",
      accessMask = "752",
      origins = None
    )))
  }

  it must "be extracted from HTTP response with missing optional fields" in {
    ResponseObject.extractFrom(ResponseBuilder(200,
      """{
        | "user": "gandalf",
        | "group": "istari",
        | "accessMask": "752"
        |}
      """.stripMargin
    )) must be (Success(ResponseObject(
      user = "gandalf",
      group = "istari",
      accessMask = "752",
      origins = None
    )))
  }

  it must "fail to be extracted from HTTP response with missing required fields" in {
    evaluating {
      ResponseObject.extractFrom(ResponseBuilder(200,
        """{
          | "user": "gandalf",
          | "origins": "anyHost"
          |}
        """.stripMargin
      )).get
    } must produce [IllegalArgumentException]
  }

  it must "fail to be extracted from HTTP response with invalid access mask value" in {
    evaluating {
      ResponseObject.extractFrom(ResponseBuilder(200,
        """{
          | "user": "gandalf",
          | "group": "istari",
          | "accessMask": "This is a invalid access mask value"
          |}
        """.stripMargin
      )).get
    } must produce [IllegalArgumentException]
  }

  it must "fail to be extracted from HTTP response with invalid format" in {
    evaluating {
      ResponseObject.extractFrom(ResponseBuilder(200,
        "Hello World! This is a stupid response that does not mean anything!"
      )).get
    } must produce [IllegalArgumentException]
  }
}
