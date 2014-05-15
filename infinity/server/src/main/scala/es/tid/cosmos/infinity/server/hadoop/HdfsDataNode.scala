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
import scala.concurrent._
import scala.concurrent.ExecutionContext.Implicits.global
import scala.math.min

import org.apache.commons.io.input.BoundedInputStream
import org.apache.hadoop.hdfs.DFSClient
import org.apache.hadoop.hdfs.client.HdfsDataInputStream

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.util.{IoUtil, ToClose}
import org.apache.hadoop.io.IOUtils

class HdfsDataNode(clientFactory: DfsClientFactory, bufferSize: Int) extends DataNode {
  import HdfsDataNode._

  override def open(
      path: Path, offset: Option[Long], length: Option[Long]): Future[ToClose[InputStream]] =
    future { withClient { client =>
      val actualOffset: Long = offset.getOrElse(NoOffset)
      val actualLength: Long = length.getOrElse(UnlimitedLength)
      val in = new HdfsDataInputStream(client.open(path.toString))
      in.seek(actualOffset)
      val readUpTo = min(actualLength, in.getVisibleLength - actualOffset)
      ToClose(new BoundedInputStream(in, readUpTo), client)
    }}


  override def append(path: Path, contentStream: InputStream): Future[Unit] =
    future { withClient { client =>
      val out = client.append(path.toString, bufferSize, NoProgress, NoStatistics)
      IoUtil.withAutoClose(Seq(contentStream, out)) {
        IOUtils.copyBytes(contentStream, out, bufferSize)
      }
    }}

  override def overwrite(path: Path, contentStream: InputStream): Future[Unit] =
    future { withClient { client =>
      val info = client.getFileInfo(path.toString)
      val out = client.create(
        path.toString,
        DoOverwrite,
        info.getReplication,
        info.getBlockSize,
        NoProgress,
        bufferSize)
      IoUtil.withAutoClose(Seq(contentStream, out)) {
        IOUtils.copyBytes(contentStream, out, bufferSize)
      }
    }}

  private def withClient[T](block: DFSClient => T): T = block(clientFactory.newClient)
}

private object HdfsDataNode {
  private val NoOffset: Long = 0
  private val UnlimitedLength: Long = -1 // unlimited length for BoundedInputStream
  private val NoProgress, NoStatistics = null
  private val DoOverwrite = true
}
