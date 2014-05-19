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

package es.tid.cosmos.infinity.server.unfiltered

import java.io.{InputStream, Reader}
import scala.concurrent.Promise

import unfiltered.{Cookie, Async}
import unfiltered.request.HttpRequest
import unfiltered.response.{HttpResponse, ResponseFunction}

case class MockRequestWithResponder[Rq, Rp](
    request: HttpRequest[Rq],
    response: HttpResponse[Rp]
  ) extends HttpRequest[Rq](request.underlying) with Async.Responder[Rp] {

  private var responsePromise: Promise[Unit] = Promise[Unit]()

  override def inputStream: InputStream = request.inputStream
  override def headerNames: Iterator[String] = request.headerNames
  override def reader: Reader = request.reader
  override def isSecure: Boolean = request.isSecure
  override def uri: String = request.uri
  override def remoteAddr: String = request.remoteAddr
  override def parameterValues(param: String): Seq[String] = request.parameterValues(param)
  override def method: String = request.method
  override def cookies: Seq[Cookie] = request.cookies
  override def protocol: String = request.protocol
  override def parameterNames: Iterator[String] = request.parameterNames
  override def headers(name: String): Iterator[String] = request.headers(name)

  override def respond(rf: ResponseFunction[Rp]): Unit = {
    _response = rf.apply(response)
    responsePromise.success()
  }

  var _response: HttpResponse[Rp] = _

  val response_> = responsePromise.future

}
