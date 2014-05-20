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

package es.tid.cosmos.infinity.server.actions

import java.io.IOException
import scala.concurrent.Future

import org.mockito.Mockito.doReturn
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.server.util.ToClose

class GetContentTest extends FlatSpec with MustMatchers with FutureMatchers {

  "Get content action" must "return Found when content exists" in new Fixture {
    doReturn(Future.successful(ToClose(in))).when(dataNode).open(on, offset = None, length = None)
    action(context) must eventually (be (ContentAction.Found(ToClose(in))))
  }

  it must "fail when there's an error in opening the file" in new Fixture {
    doReturn(Future.failed(new IOException("oops")))
      .when(dataNode).open(on, offset = None, length = None)
    action(context) must eventuallyFailWith[IOException]
  }

  trait Fixture extends ContentActionFixture {
    val action = GetContent(dataNode, on, offset = None, length = None)
  }
}
