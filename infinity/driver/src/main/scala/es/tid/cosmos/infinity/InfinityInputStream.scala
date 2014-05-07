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

import java.io.InputStream

import org.apache.hadoop.fs.{PositionedReadable, Seekable}

import es.tid.cosmos.infinity.client.InfinityClient
import es.tid.cosmos.infinity.common.fs.Path

private[infinity] class InfinityInputStream(client: InfinityClient, path: Path)
  extends InputStream with Seekable with PositionedReadable {

  override def read(): Int = ???

  override def getPos: Long = ???

  override def seekToNewSource(targetPos: Long): Boolean = ???

  override def seek(pos: Long): Unit = ???

  override def read(position: Long, buffer: Array[Byte], offset: Int, length: Int): Int = ???

  override def readFully(position: Long, buffer: Array[Byte], offset: Int, length: Int): Unit = ???

  override def readFully(position: Long, buffer: Array[Byte]): Unit = ???
}
