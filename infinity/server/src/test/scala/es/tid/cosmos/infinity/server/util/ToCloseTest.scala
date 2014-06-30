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

package es.tid.cosmos.infinity.server.util

import java.io.Closeable

import org.mockito.Mockito.inOrder
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

class ToCloseTest extends FlatSpec with MustMatchers with MockitoSugar {

  "A ToClose" must "close all resources after using the principal one" in {
    val c1 = mock[Closeable]("principal")
    val c2 = mock[Closeable]("secondary")
    val nameOfResourceUsed = ToClose(c1, c2).useAndClose(_.toString)
    val order = inOrder(c1, c2)
    nameOfResourceUsed must be ("principal")
    order.verify(c1).close()
    order.verify(c2).close()
  }

  it must "only allow to use the resource once" in {
    val toClose = ToClose(mock[Closeable])
    toClose.useAndClose(identity)
    evaluating (toClose.useAndClose(identity)) must produce [IllegalArgumentException]
  }
}
