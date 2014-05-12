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

package es.tid.cosmos.infinity.client

import java.io._

/** Wrapper of a PipedInputStream that hides piped stream details to the stream
  * consumer.
  */
class BufferedInputStream private (consumerPipe: PipedInputStream)
  extends FilterInputStream(consumerPipe) {

  def this(bufferSize: Int) = this(new PipedInputStream(bufferSize))

  /** Writing side of the pipe */
  val producerPipe: PipedOutputStream = new PipedOutputStream(consumerPipe)

  override def read(): Int = withPipeClosedAsEof(super.read())

  override def read(b: Array[Byte]): Int =  withPipeClosedAsEof(super.read(b))

  override def read(b: Array[Byte], off: Int, len: Int): Int =
    withPipeClosedAsEof(super.read(b, off, len))

  private def withPipeClosedAsEof(block: => Int): Int = try {
    block
  } catch {
    case pipeClosed: IOException if pipeClosed.getMessage == "Pipe closed" => -1
  }
}

