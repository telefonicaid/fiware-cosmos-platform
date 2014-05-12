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

package es.tid.cosmos.infinity.streams

import java.io.{IOException, InputStream}
import scala.concurrent.Await
import scala.util.{Failure, Success}

/** The stream is not being reading and can be seek without triggering requests.
  * The first read operation will trigger a transition to the ReadingStream state.
  */
object SeekingStreamState extends StreamState {

  override def seek(context: StreamContext, position: Long): Unit = {
    context.position = position
  }

  override def read(context: StreamContext, b: Array[Byte], off: Int, len: Int): Int = {
    val nextState = new ReadingStreamState(openStream(context))
    context.setState(nextState)
    nextState.read(context, b, off, len)
  }

  override def read(context: StreamContext): Int = {
    val nextState = new ReadingStreamState(openStream(context))
    context.setState(nextState)
    nextState.read(context)
  }

  private def openStream(context: StreamContext): InputStream = {
    val readAttempt = context.client.read(
      path = context.path,
      offset = Some(context.position),
      length = None,
      bufferSize = context.bufferSize
    )
    Await.ready(readAttempt, context.timeout).value.get match {
      case Success(stream) => stream
      case Failure(ex) => throw new IOException("Error opening stream", ex)
    }
  }

  override def close(context: StreamContext): Unit = {
    context.setState(ClosedStreamState)
  }
}
