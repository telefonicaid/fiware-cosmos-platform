/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.server.content

import scala.util.{Failure, Success}

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.actions.{OverwriteContent, AppendContent, GetContent}
import es.tid.cosmos.infinity.server.config.ContentServerConfig
import es.tid.cosmos.infinity.server.errors.RequestParsingException
import es.tid.cosmos.infinity.server.hadoop.DataNode
import es.tid.cosmos.infinity.server.unfiltered.request.MockHttpRequest

class HttpContentActionValidatorTest extends FlatSpec with MustMatchers {

  "A content action validator" must "parse a GetContent action" in new Fixture {
    val tryAction = validator(getRequest)
    tryAction must be (Success(GetContent(dataNode, somePath, offset = None, length = None)))
  }

  it must "correctly parse valid offset and length parameters for GetContent" in new Fixture {
    val tryAction = validator(getRequest.copy(params = Map(
      "offset" -> Seq("10"),
      "length" -> Seq("30")
    )))
    tryAction must be (Success(GetContent(dataNode, somePath, offset = Some(10), length = Some(30))))
  }

  it must "support offset = 0 for GetContent" in new Fixture {
    val tryAction = validator(getRequest.copy(params = Map(
      "offset" -> Seq("0")
    )))
    tryAction must be (Success(GetContent(dataNode, somePath, offset = Some(0), length = None)))
  }

  it must "fail on negative offset for GetContent" in new Fixture {
    val tryAction = validator(getRequest.copy(params = Map(
      "offset" -> Seq("-4")
    )))
    tryAction must be (Failure(RequestParsingException.InvalidRequestParams(
      Seq("offset"))))
  }

  it must "fail on 0 length for GetContent" in new Fixture {
    val tryAction = validator(getRequest.copy(params = Map(
      "length" -> Seq("0")
    )))
    tryAction must be (Failure(RequestParsingException.InvalidRequestParams(
      Seq("length"))))
  }

  it must "report both invalid offset and length params on GetContent" in new Fixture {
    val tryAction = validator(getRequest.copy(params = Map(
      "offset" -> Seq("-1"),
      "length" -> Seq("-1")
    )))
    tryAction must be (Failure(RequestParsingException.InvalidRequestParams(
      Seq("offset", "length"))))
  }

  it must "use first value for each param in GetContent" in new Fixture {
    val tryAction = validator(getRequest.copy(params = Map(
      "offset" -> Seq("0", "-1"),
      "length" -> Seq("-1", "4")
    )))
    tryAction must be (Failure(RequestParsingException.InvalidRequestParams(
      Seq("length"))))
  }

  it must "parse an AppendContent action" in new Fixture {
    val tryAction = validator(postRequest)
    tryAction must be (Success(AppendContent(dataNode, somePath, postRequest.inputStream)))
  }

  it must "parse an Overwrite action" in new Fixture {
    val tryAction = validator(putRequest)
    tryAction must be (Success(OverwriteContent(dataNode, somePath, putRequest.inputStream)))
  }

  it must "fail on invalid path" in new Fixture {
    val invalidUri = "/some/invalid/path"
    val tryAction = validator(getRequest.copy(uri = invalidUri))
    tryAction must be (Failure(RequestParsingException.InvalidResourcePath(invalidUri)))
  }

  trait Fixture extends MockitoSugar {
    val config = new ContentServerConfig(ConfigFactory.load())
    val dataNode = mock[DataNode]("dataNode")
    val validator = new HttpContentActionValidator(config, dataNode)
    val someUri = "/infinityfs/v1/content/some/uri"
    val somePath = Path.absolute("/some/uri")
    val baseRequest = MockHttpRequest(uri = someUri)
    val getRequest = baseRequest.copy(method = "GET")
    val postRequest = baseRequest.copy(method = "POST")
    val putRequest = baseRequest.copy(method = "PUT")
  }
}
