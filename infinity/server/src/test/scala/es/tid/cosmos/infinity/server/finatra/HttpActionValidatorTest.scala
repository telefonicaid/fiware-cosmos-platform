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

package es.tid.cosmos.infinity.server.finatra

import scalaz.{Failure, Success}

import com.twitter.finagle.http.Request
import org.jboss.netty.handler.codec.http.HttpMethod
import org.scalatest.{FlatSpec, Inside}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.server.actions.GetMetadata
import es.tid.cosmos.infinity.server.util.Path

class HttpActionValidatorTest extends FlatSpec with MustMatchers with Inside {

  "Valid HTTP Action" must "fail to extract from an unknown path" in {
    val req = Request(HttpMethod.GET, "/this/is/an/invalid/path")
    inside(HttpActionValidator(req)) {
      case Failure(InvalidAction(code, _)) => code must be (InvalidResourcePath.code)
    }
  }

  it must "extract a GetPathMetadataAction" in {
    val req = Request(HttpMethod.GET, "/infinityfs/v1/metadata/path/to/file")
    HttpActionValidator(req) must be (Success(GetMetadata(Path.absolute("/path/to/file"))))
  }
}
