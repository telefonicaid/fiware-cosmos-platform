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

import com.twitter.concurrent.Broker
import com.twitter.finagle.builder.{Server, ServerBuilder}
import com.twitter.finagle.{Filter, Service}
import com.twitter.finagle.stream.{Stream, StreamResponse, EOF}
import com.twitter.util.Future
import java.net.{URI, InetSocketAddress}
import org.jboss.netty.buffer.ChannelBuffer
import org.jboss.netty.buffer.ChannelBuffers.copiedBuffer
import org.jboss.netty.handler.codec.http._
import java.io.{File, FileInputStream, InputStream}
import scala.util.matching.Regex
import scala.Some
import com.twitter.finagle.http.{Request => FinagleRequest}

/**
 * TODO: Insert description here
 *
 * @author adamos
 */
class ContentStreamServer(port: Int, basePath: Regex) {

  private var server: Option[Server] = None

  private[this] val nettyToFinagle =
    Filter.mk[HttpRequest, StreamResponse, FinagleRequest, StreamResponse] { (req, service) =>
      service(FinagleRequest(req))
    }

  private[this] lazy val service = {
    val contentService = new Service[FinagleRequest, StreamResponse] {
      def apply(request: FinagleRequest) = Future {
        val path = new URI(request.getUri).getPath
        val method = request.getMethod
        (method, path) match {
          case (HttpMethod.GET, basePath(subPath)) => getContent(subPath, responseHeader(request))
        }
      }
    }
    nettyToFinagle andThen contentService
  }

  def start(): Unit = {
    server = Some(ServerBuilder()
      .codec(Stream())
      .bindTo(new InetSocketAddress(port))
      .name("infinity_content_server")
      .build(service))
  }

  def stop(): Unit = {
    server.map(_.close())
  }

  private def getContent(path: String, responseHeader: HttpResponse): StreamResponse = {
    println("processing path " + path)
//    GetContent(null, null, null).apply()
    val in: InputStream = new FileInputStream(new File(s"/Users/adamos/dev/workspaces/cosmos/cosmos-platform/infinity/server/src/main/scala/es/tid/cosmos/infinity/server/finagle/$path"))
    new InputStreamResponse(in, responseHeader)
  }

  private def responseHeader(request: HttpRequest): HttpResponse =
    new DefaultHttpResponse(request.getProtocolVersion, HttpResponseStatus.OK)

}

object Test {
  def main(args: Array[String]) {
    val server = new ContentStreamServer(8080, "/infinityfs/v1/content/(.*)".r)
    server.start()
  }
}

class InputStreamResponse(in: InputStream, header: HttpResponse) extends StreamResponse {
  private val msgs = new Broker[ChannelBuffer]
  private  val errors = new Broker[Throwable]

  override val httpResponse = header

  override def messages = msgs.recv

  override def error = errors.recv

  override def release() = {
    println("RELEASE")
    messages foreach { b => println(b.getChar(0))}
    error foreach { e => println(e.getMessage)}
  }

  private def fromStream(): Unit = Future {
    Thread.sleep(50)
    val bufferSize = 16
    val buffer = new Array[Byte](bufferSize)
    val result = in.read(buffer, 0, bufferSize)
    val m = copiedBuffer(buffer)
    if (result > -1) msgs.send(m) andThen fromStream()
    else errors.send(EOF).sync()
  }

  fromStream()
}
