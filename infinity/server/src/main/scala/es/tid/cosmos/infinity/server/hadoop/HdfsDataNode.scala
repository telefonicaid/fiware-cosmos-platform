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

package es.tid.cosmos.infinity.server.hadoop

import java.io.InputStream
import scala.math.min

import org.apache.commons.io.input.BoundedInputStream
import org.apache.hadoop.io.IOUtils
import org.apache.hadoop.hdfs.DFSClient
import org.apache.hadoop.hdfs.client.HdfsDataInputStream
import org.apache.hadoop.hdfs.protocol.HdfsFileStatus

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.hadoop.HdfsDataNode._
import es.tid.cosmos.infinity.server.util.ToClose
import es.tid.cosmos.infinity.server.util.IoUtil._

/** Constructor exposing boundedStreamFactory for testability */
class HdfsDataNode private[hadoop] (
    clientFactory: DfsClientFactory,
    bufferSize: Int,
    boundedStreamFactory: BoundedStreamFactory) extends DataNode {

  def this(clientFactory: DfsClientFactory, bufferSize: Int) =
    this(clientFactory, bufferSize, defaultStreamFactory)

  override def open(path: Path, offset: Option[Long], length: Option[Long]): ToClose[InputStream] =
    clientFactory.withFailsafeClient { client =>
      checkAndGetValidFileInfo(path, client)
      val actualOffset: Long = offset.getOrElse(NoOffset)
      val in = new HdfsDataInputStream(client.open(path.toString))
      withAutoCloseOnFail(in) {
        in.seek(actualOffset)
        val visibleFileLength = in.getVisibleLength - actualOffset
        val readUpTo = length.map(l => min(l, visibleFileLength)).getOrElse(visibleFileLength)
        ToClose(boundedStreamFactory(in, readUpTo), client)
      }
    }

  override def append(path: Path, contentStream: InputStream): Unit =
    clientFactory.withFailsafeClient { client =>
      withAutoClose(contentStream, client) {
        checkAndGetValidFileInfo(path, client)
        val out = client.append(path.toString, bufferSize, NoProgress, NoStatistics)
        withAutoClose(out) {
          IOUtils.copyBytes(contentStream, out, bufferSize)
        }
      }
    }

  override def overwrite(path: Path, contentStream: InputStream): Unit =
    clientFactory.withFailsafeClient { client =>
      withAutoClose(contentStream, client) {
        val info = checkAndGetValidFileInfo(path, client)
        val out = client.create(
          path.toString,
          DoOverwrite,
          info.getReplication,
          info.getBlockSize,
          NoProgress,
          bufferSize)
        withAutoClose(out) {
          IOUtils.copyBytes(contentStream, out, bufferSize)
        }
      }
    }

  private def checkAndGetValidFileInfo(path: Path, client: DFSClient): HdfsFileStatus = {
    val info = Option(client.getFileInfo(path.toString))
      .getOrElse(throw DataNodeException.FileNotFound(path))
    if (info.isDir) throw DataNodeException.ContentPathIsDirectory(path)
    else info
  }
}

private[hadoop] object HdfsDataNode {
  /** Factory signature for offering a stream with access to limited length */
  type BoundedStreamFactory = (InputStream, Long) => InputStream
  val DoOverwrite = true
  val NoProgress, NoStatistics = null
  val NoOffset: Long = 0

  private def defaultStreamFactory(in: InputStream, length: Long) = {
    require(length >= 0, s"Length [$length] must be a positive number")
    new BoundedInputStream(in, length)
  }
}
