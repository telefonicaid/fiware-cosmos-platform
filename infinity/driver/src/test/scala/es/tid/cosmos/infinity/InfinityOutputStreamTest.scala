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

package es.tid.cosmos.infinity

import java.io.ByteArrayOutputStream

import org.apache.hadoop.util.Progressable
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class InfinityOutputStreamTest extends FlatSpec with MustMatchers {

  "An infinity output stream" must "decorate an output stream" in {
    val wrapped = new ByteArrayOutputStream()
    val stream = new InfinityOutputStream(wrapped, progress = None)
    stream.write("Conten".getBytes)
    stream.write("t".charAt(0))
    stream.close()
    new String(wrapped.toByteArray) must be ("Content")
  }

  it must "report progress on stream operations" in {
    var count = 0
    val wrapped = new ByteArrayOutputStream()
    val stream = new InfinityOutputStream(wrapped, progress = Some(new Progressable {
      override def progress(): Unit = { count = count + 1 }
    }))
    stream.write("Conten".getBytes)
    stream.write("t".charAt(0))
    stream.flush()
    stream.close()
    count must be (4)
  }
}
