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

import java.io.{Closeable, OutputStream, InputStream}
import java.util
import scala.annotation.tailrec
import scala.math.min

import unfiltered.response.ResponseStreamer

import es.tid.cosmos.infinity.server.util.IoUtil

case class ResponseInputStream(
    in: InputStream,
    maxChunkSize: Int,
    length: Long,
    closeables: Seq[Closeable]) extends ResponseStreamer {

  import ResponseInputStream._
  lazy val buffer = new Array[Byte](maxChunkSize)

  override def stream(out: OutputStream): Unit = IoUtil.withAutoClose((in +: closeables).distinct) {
    @tailrec
    def transfer(leftToRead: Long): Unit = {
      val chunkSize = min(maxChunkSize, leftToRead).toInt
      cleanBuffer()
      val bytesRead: Int = in.read(buffer, NoOffset, chunkSize)
      val nextLeftToRead = leftToRead - bytesRead
      if (gotContentFromStream(bytesRead)) {
        out.write(buffer, NoOffset, bytesRead)
        out.flush()
        if (moreLeftToRead(nextLeftToRead)) transfer(nextLeftToRead)
      }
    }

    transfer(length)
  }

  private def cleanBuffer(): Unit = {
    util.Arrays.fill(buffer, NullByte)
  }
}

object ResponseInputStream {
  private val NoOffset = 0
  private val NullByte: Byte = 0
  private def gotContentFromStream(bytesRead: Int) = bytesRead > -1
  private def moreLeftToRead(leftToRead: Long) = leftToRead > 0
}
