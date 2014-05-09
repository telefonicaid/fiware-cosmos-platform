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

package es.tid.cosmos.infinity.server.finagle

import java.io.{Closeable, InputStream}

import com.twitter.concurrent.Broker
import com.twitter.finagle.http.Response
import com.twitter.finagle.stream.{EOF, StreamResponse}
import com.twitter.util.{Future => TwitterFuture}
import org.jboss.netty.buffer.ChannelBuffer
import org.jboss.netty.buffer.ChannelBuffers.copiedBuffer

import es.tid.cosmos.infinity.server.util.IoUtil

/** Chunked stream response with contents from an input stream.
  *
  * @param in         the input stream where the content will be read from
  * @param chunkSize  the size of each chunk that the content will be split to
  * @param length     the number of bytes to read from the input stream
  * @param closeables all the resources involved in reading this stream that should be closed
  *                   after reading it. This should include the input stream itself
  * @param header     the response whose header will be used to form the streaming response
  */
class InputStreamResponse(
    in: InputStream,
    chunkSize: Int,
    length: Long,
    closeables: Seq[Closeable],
    header: Response) extends StreamResponse {
  import InputStreamResponse._

  require(closeables.contains(in),
    "Input Stream should also be provided in closeables " +
      "so that the resource can be released after being read.")

  private val msgs = new Broker[ChannelBuffer]
  private  val errors = new Broker[Throwable]

  override val httpResponse = header

  override def messages = msgs.recv

  override def error = errors.recv

  override def release() = {
    // Ignore messages and errors once the response has been released
    messages foreach { _ => ()}
    error foreach { _ => ()}
  }

  private def streamNextChunk(): Unit = TwitterFuture {
    val buffer = new Array[Byte](chunkSize)
    val bytesRead: Int = in.read(buffer, NoOffset, chunkSize)
    val chunk = copiedBuffer(buffer)
    if (bytesRead > NoBytesLeft)
      msgs.send(chunk) andThen streamNextChunk()
    else
      errors.send(EOF).sync()
  }

  IoUtil.withAutoClose(closeables) { streamNextChunk() }
}

private object InputStreamResponse {
  val NoOffset = 0
  val NoBytesLeft = -1
}
