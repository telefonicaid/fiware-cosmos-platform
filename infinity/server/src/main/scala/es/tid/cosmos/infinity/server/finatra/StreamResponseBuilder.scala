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

package es.tid.cosmos.infinity.server.finatra

import java.io.{OutputStream, InputStream}

import com.twitter.finatra.{Request, ResponseBuilder}
import com.twitter.finagle.http.{Response => FinagleResponse}
import org.jboss.netty.buffer.{ChannelBufferOutputStream, ChannelBuffers}
import org.apache.hadoop.io.IOUtils

/**
 * TODO: Insert description here
 *
 * @author adamos
 */
class StreamResponseBuilder extends ResponseBuilder {

  private var bodyStream: Option[InputStream] = None
  private var length: Option[Long] = None

  override def build(request: Request): FinagleResponse = {
    val original = super.build(request)
    if (original.getContentString().isEmpty) replaceWithStreamBody(original)
    else original
  }

  def body(stream: InputStream, length: Long): StreamResponseBuilder = {
    this.bodyStream = Some(stream)
    this.length = Some(length)
    this
  }

  private def replaceWithStreamBody(response: FinagleResponse): FinagleResponse = {
    combine(bodyStream, length) foreach { case (stream, upTo) =>
      val channelBuffer = ChannelBuffers.dynamicBuffer(4096)    //IOUtils uses 4096. Should we be fixing it?
      val out: OutputStream = new ChannelBufferOutputStream(channelBuffer)
      response.headers().set("Content-Length", upTo)
      response.setContent(channelBuffer)
      IOUtils.copyBytes(stream, out, upTo, false)
    }
    response
  }

  private def combine[L, R](left: Option[L], right: Option[R]): Option[(L, R)] = (left, right) match {
    case (Some(l), Some(r)) => Some((l, r))
    case _ => None
  }
}

