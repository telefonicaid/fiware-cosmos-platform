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

import java.io.{ByteArrayOutputStream, OutputStream, ByteArrayInputStream}

import java.nio.charset.Charset

import org.mockito.Mockito._
import org.mockito.Matchers._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

class ResponseInputStreamTest extends FlatSpec with MustMatchers with MockitoSugar {
  "An input stream response" must "write the response in chunks according to given max size" in
    new Fixture {
      val response = ResponseInputStream(
        in,
        maxChunkSize = 4,
        length = 8,
        closeables = Seq(in, out)
      )
      response.stream(out)
      val order = inOrder(out)
      order.verify(out).write("1234".getBytes)
      order.verify(out).write("5678".getBytes)
      order.verify(out, never()).write(any[Array[Byte]])
    }

  it must "contain only the specified length" in new Fixture {
    val response = ResponseInputStream(
      in,
      maxChunkSize = 4,
      length = 5,
      closeables = Seq(in, out)
    )
    response.stream(out)
    val order = inOrder(out)
    order.verify(out).write("1234".getBytes)
    order.verify(out).write("5".getBytes)
    order.verify(out, never()).write(any[Array[Byte]])
  }

  it must "close all used resources in the end" in {
    val _in = new ByteArrayInputStream(content.getBytes)
    try {
      val in = spy(_in)
      val out = mock[OutputStream]
      val response = ResponseInputStream(
        in,
        maxChunkSize = 4,
        length = 8,
        closeables = Seq(in, out)
      )
      response.stream(out)
      verify(in).close()
      verify(out).close()
    } finally {
      _in.close()
    }
  }

  it must "not fail when attempting to read beyond the stream's length" in new Fixture {
    override val out = new ByteArrayOutputStream()
    val response = ResponseInputStream(
      in,
      maxChunkSize = 4,
      length = 9,
      closeables = Seq(in, out)
    )
    response.stream(out)
    out.toString(Charset.defaultCharset().toString) must be (content)
  }

  val content = "12345678"

  trait Fixture {
    val in = new ByteArrayInputStream(content.getBytes)
    val out = mock[OutputStream]
  }
}
