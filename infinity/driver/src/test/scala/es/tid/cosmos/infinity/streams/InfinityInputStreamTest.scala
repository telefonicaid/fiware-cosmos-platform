/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.streams

import java.io.IOException
import scala.concurrent.duration._

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.MockInfinityClient
import es.tid.cosmos.infinity.common.fs.RootPath

class InfinityInputStreamTest extends FlatSpec with MustMatchers {

  "An infinity input stream" must "throw when reading after being closed" in new Fixture {
    stream.close()
    stream.seek(10)
    stream.getPos must be (10)
    evaluating { stream.read() } must produce [IOException]
    evaluating { stream.read(buffer, 0, buffer.size) } must produce [IOException]
    evaluating { stream.close() } must produce [IOException]
  }

  it must "read a file" in new Fixture {
    client.givenFileWithContent(path, content)
    stream.readFully(0, buffer)
    stream.close()
    new String(buffer) must be (content)
  }

  it must "read a file char by char" in new Fixture {
    client.givenFileWithContent(path, content)
    val bytes = Stream.continually(stream.read()).takeWhile(_ != -1).map(_.toByte).toArray
    stream.close()
    new String(bytes) must be (content)
  }

  it must "seek to read at any point" in new Fixture {
    client.givenFileWithContent(path, content)
    stream.seek(9L)
    stream.read() must be (content.charAt(9))
    stream.seek(10L)
    stream.read() must be (content.charAt(10))
    stream.close()
  }

  it must "track position within the stream" in new Fixture {
    client.givenFileWithContent(path, content)
    var pos = 0
    while (pos < 3) {
      if (stream.read() != -1) {
        pos += 1
      }
    }
    while (pos < 6) {
      val readBytes = stream.read(buffer, 0, 1)
      if (readBytes != -1) {
        pos += readBytes
      }
    }
    stream.getPos must be (6)
  }

  it must "fail when reading if client can't connect with infinity server" in new Fixture {
    client.givenFileReadWillFail(path)
    evaluating {
      stream.read()
    } must produce [IOException]
  }

  it must "not support seeking to new sources" in new Fixture {
    stream.seekToNewSource(0) must be (false)
  }

  val path = RootPath / "some" / "file"
  val timeout = 200.millis
  val content = "01234567890123456789"
  val bufferSize = 1024

  trait Fixture {
    val client = new MockInfinityClient
    val stream = new InfinityInputStream(client.value, path, bufferSize, timeout)
    val buffer = new Array[Byte](20)
  }
}
