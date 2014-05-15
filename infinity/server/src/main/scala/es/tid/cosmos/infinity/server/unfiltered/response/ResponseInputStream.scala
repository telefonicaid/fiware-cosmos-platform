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

package es.tid.cosmos.infinity.server.unfiltered.response

import java.io.{OutputStream, InputStream}
import java.util
import scala.annotation.tailrec

import unfiltered.response.ResponseStreamer

 /** An HTTP response for Unfiltered that creates the response body in chunks reading it from
   * the given input stream. <b>Note:</b> The response is not responsible for closing the given
   * stream. Closing the stream is left to the client's discretion.
   *
   * @param in           the stream from where to read the response body
   * @param maxChunkSize the maximum size of each chunk
   */
case class ResponseInputStream(
    in: InputStream,
    maxChunkSize: Int) extends ResponseStreamer {

  import ResponseInputStream._
  lazy val mutableBuffer = new Array[Byte](maxChunkSize)

  override def stream(out: OutputStream): Unit = {
    // note: out is automatically closed by ResponseStreamer.respond
    @tailrec
    def transfer(): Unit = {
      cleanBuffer()
      val bytesRead: Int = in.read(mutableBuffer, NoOffset, maxChunkSize)
      if (gotContentFromStream(bytesRead)) {
        out.write(mutableBuffer, NoOffset, bytesRead)
        out.flush()
        transfer()
      }
    }

    transfer()
  }

  private def cleanBuffer(): Unit = {
    util.Arrays.fill(mutableBuffer, NullByte)
  }
}

object ResponseInputStream {
  private val NoOffset = 0
  private val NullByte: Byte = 0
  private def gotContentFromStream(bytesRead: Int) = bytesRead > -1
}
