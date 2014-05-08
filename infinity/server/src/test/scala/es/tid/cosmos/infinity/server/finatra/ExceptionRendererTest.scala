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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import es.tid.cosmos.infinity.server.authentication.AuthenticationException
import es.tid.cosmos.infinity.server.actions.NameNodeException
import es.tid.cosmos.infinity.common.fs.Path

class ExceptionRendererTest extends FlatSpec with MustMatchers {

  "Exception renderer" must "render MissingAuthorizationHeader" in {
    givenException(new RequestParsingException.MissingAuthorizationHeader())
      .mustRenderTo(401, "REQ01")
  }

  it must "render InvalidResourcePath" in {
    givenException(new RequestParsingException.InvalidResourcePath("/foobar"))
      .mustRenderTo(404, "REQ02")
  }

  it must "render UnsupportedAuthorizationHeader" in {
    givenException(new RequestParsingException.UnsupportedAuthorizationHeader("blabla"))
      .mustRenderTo(401, "REQ03")
  }

  it must "render MalformedKeySecretPair" in {
    givenException(new RequestParsingException.MalformedKeySecretPair("blabla"))
      .mustRenderTo(401, "REQ04")
  }

  it must "render InvalidBasicHash" in {
    givenException(new RequestParsingException.InvalidBasicHash("blabla"))
      .mustRenderTo(401, "REQ05")
  }

  it must "render AuthenticationException" in {
    givenException(new AuthenticationException("blabla"))
      .mustRenderTo(401, "SEC01")
  }

  it must "render Unauthorized" in {
    givenException(NameNodeException.Unauthorized(Path.absolute("/path")))
      .mustRenderTo(403, "SEC02")
  }

  it must "render IOError" in {
    givenException(NameNodeException.IOError(null))
      .mustRenderTo(500, "SRV01")
  }

  it must "render NoSuchPath" in {
    givenException(NameNodeException.NoSuchPath(Path.absolute("/path")))
      .mustRenderTo(404, "CONST01")
  }

  it must "render PathAlreadyExists" in {
    givenException(NameNodeException.PathAlreadyExists(Path.absolute("/path")))
      .mustRenderTo(409, "CONST02")
  }

  it must "render ParentNotDirectory" in {
    givenException(NameNodeException.ParentNotDirectory(Path.absolute("/path")))
      .mustRenderTo(422, "CONST03")
  }

  case class givenException(e: Throwable) {
    def mustRenderTo(statusCode: Int, errorCode: String) = {
      val rep = ExceptionRenderer(e).build
      rep.getStatusCode() must be (statusCode)
      rep.getContentString() must include (errorCode)
    }
  }
}
