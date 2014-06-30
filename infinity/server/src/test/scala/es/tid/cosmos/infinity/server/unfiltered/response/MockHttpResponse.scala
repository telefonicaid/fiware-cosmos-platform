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

import java.nio.charset.Charset
import java.io.{OutputStream, ByteArrayOutputStream}

import unfiltered.Cookie
import unfiltered.response.HttpResponse

class MockHttpResponse[R](underlying: R) extends HttpResponse[R](underlying) {
  private val _out = new ByteArrayOutputStream()
  var _status: Int = -1
  var _headers: Map[String, String] = Map.empty
  lazy val body: String = _out.toString(Charset.defaultCharset().toString)

  override def status(statusCode: Int): Unit =
    _status = statusCode

  override def header(name: String, value: String): Unit =
    _headers = _headers.updated(name, value)

  override def outputStream: OutputStream = _out

  override def redirect(url: String): Unit = ???
  override def cookies(cookie: Seq[Cookie]): Unit = ???
}
