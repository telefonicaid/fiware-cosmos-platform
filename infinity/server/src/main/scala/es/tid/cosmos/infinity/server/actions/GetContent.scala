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
import es.tid.cosmos.infinity.server.actions.Action.{Result, Context}
import org.apache.hadoop.hdfs.DFSClient
import org.apache.hadoop.conf.Configuration
import java.net.URI
import org.apache.hadoop.hdfs.client.HdfsDataInputStream
import org.apache.hadoop.io.IOUtils
import java.io.OutputStream
import com.twitter.finagle.http.Response
import org.jboss.netty.buffer.{ChannelBufferOutputStream, ChannelBuffers}

/**
 * TODO: Insert description here
 *
 */
case class GetContent(dataNode: DataNode, on: Path, offset: Option[Long], length: Option[Long]) extends Action {
  // TODO: Figure out where to do offset and length checking for positive numbers
  import ExecutionContext.Implicits.global

  val actualOffset: Long = offset.getOrElse(0)
  val actualLength: Long = length.getOrElse(Long.MaxValue)

  override def apply(context: Context): Future[Result] = future {
    val conf = new Configuration(dataNode.getConf)
    val nnAddress: URI = null
    val client = new DFSClient(nnAddress, conf)
    val in = new HdfsDataInputStream(client.open(on.toString))
    in.seek(actualOffset)
    val upTo = min(actualLength, in.getVisibleLength - actualOffset)
    //use the upTo to copy the in stream to the out stream from offset to upto
    val channelBuffer = ChannelBuffers.dynamicBuffer(4096)    //IOUtils uses 4096. Should we be fixing it?
    val out: OutputStream = new ChannelBufferOutputStream(channelBuffer)
    IOUtils.copyBytes(in, out, upTo, false)

    val response = Response()
    response.setChunked(true)
    response.setContent(channelBuffer)
    // need to do clean up of closeables like DFSClient
    ???
  }
}
