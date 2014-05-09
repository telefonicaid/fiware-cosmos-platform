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

import com.twitter.concurrent.{Broker, Offer}
import com.twitter.finagle.stream.{EOF, StreamResponse}
import com.twitter.finatra.ResponseBuilder
import org.jboss.netty.buffer.ChannelBuffer
import org.jboss.netty.handler.codec.http.HttpResponse

/** Method extensions for converting normal responses to StreamResponse */
object StreamConversions {
  implicit class BlingStreamReponse(val response: ResponseBuilder) extends AnyVal {

    /** Build a Finagle response with the builder and convert it to a [[StreamResponse]] */
    def toStream: StreamResponse = new StreamResponse {
      private val internalResponse = response.build
      private val msgs = new Broker[ChannelBuffer]
      private  val errors = new Broker[Throwable]

      override val httpResponse: HttpResponse = internalResponse

      override def messages: Offer[ChannelBuffer] = msgs.recv

      override def error: Offer[Throwable] = errors.recv

      override def release(): Unit = ()

      msgs.send(internalResponse.getContent()) andThen errors.send(EOF).sync()
    }
  }
}
