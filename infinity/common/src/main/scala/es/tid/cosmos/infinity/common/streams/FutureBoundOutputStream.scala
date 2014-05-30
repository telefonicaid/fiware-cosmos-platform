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

import java.io.OutputStream
import scala.concurrent.Future
import scala.concurrent.duration.FiniteDuration

import es.tid.cosmos.infinity.common.util.TimeBound

/** Wrapper of an output stream, bounding its release to a given Future.
  *
  * @param out       the output stream to wrap
  * @param request_> the future to bound on
  * @param timeOut   the time limit to wait for the future to complete
  */
class FutureBoundOutputStream(
      out: OutputStream, request_> : Future[_], timeOut: FiniteDuration) extends OutputStream {

  private val timeBound = new TimeBound(timeOut)

  override def write(p1: Int): Unit = out.write(p1)
  override def write(a: Array[Byte]): Unit = out.write(a)
  override def write(a: Array[Byte], offset: Int, length: Int): Unit = out.write(a, offset, length)
  override def flush(): Unit = out.flush()

  /** Close the stream and block until the future completes. */
  override def close(): Unit = {
    out.close()
    timeBound.awaitResult(request_>)
  }
}
