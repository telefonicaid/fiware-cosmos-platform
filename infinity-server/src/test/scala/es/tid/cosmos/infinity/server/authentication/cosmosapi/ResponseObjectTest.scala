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
