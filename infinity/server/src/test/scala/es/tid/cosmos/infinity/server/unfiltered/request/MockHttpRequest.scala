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

package es.tid.cosmos.infinity.server.unfiltered.request

import java.io.{InputStream, Reader}

import org.scalatest.mock.MockitoSugar
import unfiltered.Cookie
import unfiltered.request.HttpRequest


case class MockHttpRequest[R](
    override val underlying: R = null,
    inputStream: InputStream = MockHttpRequest.mockStream,
    reader: Reader = MockHttpRequest.mockReader,
    headerNames: Iterator[String] = Seq.empty.iterator,
    isSecure: Boolean = false,
    uri: String = "",
    remoteAddr: String = "",
    method: String = "",
    cookies: Seq[Cookie] = Seq.empty,
    protocol: String = "",
    params: Map[String, Seq[String]] = Map.empty) extends HttpRequest[R](underlying) {

  def headers(name: String): Iterator[String] = Seq.empty.iterator
  def parameterValues(param: String): Seq[String] = params(param)
  val parameterNames: Iterator[String] = params.keysIterator
}

object MockHttpRequest extends MockitoSugar {
  def mockStream = mock[InputStream]
  def mockReader = mock[Reader]
}
