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
import scala.util.{Failure, Try}

class IoUtilTest extends FlatSpec with MustMatchers with MockitoSugar {

  "Auto Close" must "execute the block and close the closeables in sequence" in new Fixture {
    val result = IoUtil.withAutoClose(Seq(c1, c2)) {
      c1.toString + c2.toString
    }
    result must not be 'empty
    order.verify(c1).close()
    order.verify(c2).close()
  }

  it must "close the closeables in sequence when block fails" in new Fixture {
    val exception = new RuntimeException("Oops!")
    val result = Try(IoUtil.withAutoClose(Seq(c1, c2)) {
      throw exception
    })
    result must be (Failure(exception))
    order.verify(c1).close()
    order.verify(c2).close()
  }

  trait Fixture {
    val c1, c2 = mock[Closeable]
    val order = inOrder(c1, c2)
  }
}
