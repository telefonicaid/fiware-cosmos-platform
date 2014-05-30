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

package es.tid.cosmos.infinity.common.streams

import java.io.{IOException, OutputStream}
import scala.concurrent.{Future, Promise, blocking}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._

import org.mockito.Mockito.verify
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers

class FutureBoundOutputStreamTest
    extends FlatSpec with MustMatchers with MockitoSugar with FutureMatchers {
  
  "A future bounded output stream" must "block until future succeeds when closing" in new Fixture {
    var sideEffect = false
    val close_> = Future { blocking {
      out.close()
      sideEffect = true
    }}
    close_>.isCompleted must be (false)
    sideEffect must be (false)
    promise.success()
    close_> must eventuallySucceed
    verify(_out).close()
    sideEffect must be (true)
  }

  it must "fail when future does not complete within timeout" in new Fixture {
    val close_> = Future { blocking { out.close() } }
    close_>.isCompleted must be (false)
    close_> must eventuallyFailWith[IOException]
    verify(_out).close()
  }

  it must "fail with an IOException when future fails" in new Fixture {
    promise.failure(new RuntimeException("oops"))
    val close_> = Future { blocking { out.close() } }
    close_>.isCompleted must be (false)
    close_> must eventuallyFailWith[IOException]
    verify(_out).close()
  }

  val timeOut = 2.seconds

  trait Fixture {
    val _out = mock[OutputStream]
    val promise = Promise[Unit]()
    val out = new FutureBoundOutputStream(_out, promise.future, timeOut)
  }
}
