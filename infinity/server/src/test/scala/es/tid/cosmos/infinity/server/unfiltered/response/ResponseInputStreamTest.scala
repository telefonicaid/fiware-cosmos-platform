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

import java.io.{InputStream, OutputStream, ByteArrayInputStream}

import org.mockito.Mockito.{spy, inOrder}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.server.util.ToClose

class ResponseInputStreamTest extends FlatSpec with MustMatchers with MockitoSugar {

  //TODO: Ensure there exists test with regards to bounded read.
  // This is now an part of the HdfsDataNode implementation

  "An input stream response" must "write the response in chunks according to given max size" in
    new Fixture {
      val response = ResponseInputStream(
        in,
        maxChunkSize = 4)
      response.stream(out)
      out.chunks must be (Seq(
        ChunkWrites("1234".getBytes, 0, 4),
        ChunkWrites("5678".getBytes, 0, 4),
        ChunkWrites(Seq("9".getBytes.apply(0), 0, 0, 0), 0, 1)
      ))
    }

  it must "close the input stream and related resources after reading all the content" in
    new Fixture{
      val response = ResponseInputStream(
        in,
        maxChunkSize = 10)
      response.stream(out)
      out.chunks must be (Seq(ChunkWrites(content.getBytes ++ Seq[Byte](0), 0, 9)))
      val order = inOrder(internalIn, otherResource)
      order.verify(internalIn).close()
      order.verify(otherResource).close()
    }

  val content = "123456789"

  trait Fixture {
    val otherResource = mock[InputStream]
    val internalIn = spy(new ByteArrayInputStream(content.getBytes))
    val in = ToClose(internalIn, otherResource)
    val out = new MockOutputStream
  }

  class MockOutputStream extends OutputStream {
    var chunks: Seq[ChunkWrites] = Seq.empty
    override def write(i: Int): Unit = ???
    override def write(bytes: Array[Byte], offset: Int, length: Int): Unit = {
      println(bytes.toSeq)
      chunks :+= ChunkWrites(Seq.empty ++ bytes.toSeq, offset, length)
    }
    override def flush(): Unit = ()
    override def close(): Unit = ()
  }

  case class ChunkWrites(bytes: Seq[Byte], offset: Int, length: Int)
}
