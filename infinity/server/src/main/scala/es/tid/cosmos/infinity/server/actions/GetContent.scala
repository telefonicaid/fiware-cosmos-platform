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

package es.tid.cosmos.infinity.server.actions

import scala.concurrent._
import scala.math.min

import org.apache.hadoop.hdfs.server.datanode.DataNode

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.actions.Action.{ContentFound, Result, Context}
import org.apache.hadoop.hdfs.DFSClient
import org.apache.hadoop.conf.Configuration
import java.net.URI
import org.apache.hadoop.hdfs.client.HdfsDataInputStream
import org.apache.hadoop.io.IOUtils
import java.io.OutputStream
import com.twitter.finagle.http.Response
import org.jboss.netty.buffer.{ChannelBufferOutputStream, ChannelBuffers}
import es.tid.cosmos.infinity.server.hadoop.DFSClientFactory

/**
 * TODO: Insert description here
 *
 */
case class GetContent(dfsClientFactory: DFSClientFactory, on: Path, offset: Option[Long], length: Option[Long]) extends Action {
  // TODO: Figure out where to do offset and length checking for positive numbers
  import ExecutionContext.Implicits.global

  val actualOffset: Long = offset.getOrElse(0)
  val actualLength: Long = length.getOrElse(Long.MaxValue)

  override def apply(context: Context): Future[Result] = future {
    dfsClientFactory.withNewClient { client =>
      val in = new HdfsDataInputStream(client.open(on.toString))
      in.seek(actualOffset)
      val upTo = min(actualLength, in.getVisibleLength - actualOffset)
      ContentFound(in, upTo)
    }
  }
}
