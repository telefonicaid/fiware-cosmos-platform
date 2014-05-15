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

import org.mockito.BDDMockito.given
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers

class OverwriteContentTest extends FlatSpec with MustMatchers with FutureMatchers {

  "Overwrite content action" must "return Overwritten when content was successfully written" in
    new Fixture {
      given(dataNode.overwrite(on, in)).willReturn(Future.successful())
      action(context) must eventually (be (ContentAction.Overwritten(on)))
    }

  it must "fail when there's an error in writing to the file" in new Fixture {
    given(dataNode.overwrite(on, in)).willReturn(Future.failed(new IOException("oops")))
    action(context) must eventuallyFailWith[IOException]
  }

  trait Fixture extends ContentActionFixture {
    val action = OverwriteContent(dataNode, on, in)
  }
}
