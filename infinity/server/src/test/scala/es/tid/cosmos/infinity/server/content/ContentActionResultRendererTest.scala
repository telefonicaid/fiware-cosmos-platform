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

import java.io.ByteArrayInputStream
import java.nio.charset.Charset

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import unfiltered.response.{InternalServerError, NoContent, Ok}

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.actions.ContentAction
import es.tid.cosmos.infinity.server.unfiltered.response.MockHttpResponse
import es.tid.cosmos.infinity.server.util.ToClose
import es.tid.cosmos.infinity.server.unfiltered.response.MockHttpResponse

class ContentActionResultRendererTest extends FlatSpec with MustMatchers with MockitoSugar {
  "Content result Renderer" must "render Found" in new Fixture {
    val in = new ByteArrayInputStream("1234".getBytes)
    val responseFunction = render(ContentAction.Found(ToClose(in)))
    responseFunction(baseResponse)
    baseResponse._status must be (Ok.code)
    baseResponse.body must be ("1234")
  }

  it must "render Appended" in new Fixture {
    val responseFunction = render(ContentAction.Appended(path))
    responseFunction(baseResponse)
    baseResponse._status must be (NoContent.code)
  }

  it must "render Overwritten" in new Fixture {
    val responseFunction = render(ContentAction.Overwritten(path))
    responseFunction(baseResponse)
    baseResponse._status must be (NoContent.code)
  }

  it must "fail for unknown action" in new Fixture {
    val responseFunction = render(mock[ContentAction.Result]("unknown"))
    responseFunction(baseResponse)
    baseResponse._status must be (InternalServerError.code)
  }

  trait Fixture {
    val render = new ContentActionResultRenderer(chunkSize = 4)
    val path = Path.absolute("/some/file")
    val baseResponse = new MockHttpResponse
  }
}
