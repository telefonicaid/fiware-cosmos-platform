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

import scala.concurrent.duration.FiniteDuration

import org.apache.hadoop.fs.FSInputStream

import es.tid.cosmos.infinity.client.InfinityClient
import es.tid.cosmos.infinity.common.fs.SubPath

/** Input stream able to perform several request though an infinity client when the stream
  * is seek back and forth.
  *
  * The stream is implemented using a state pattern to isolate the logic of the several
  * client requests.
  *
  * No request is performed until the first read is called as Hadoop will open the stream, seek
  * to the desired part and then start reading.  This strategy is inspired on the analogous class
  * of WebHdfsFileSystem, [[org.apache.hadoop.hdfs.ByteRangeInputStream]].
  *
  * @param client      Infinity client
  * @param path        File to access to
  * @param bufferSize  Size of the reception buffer
  * @param timeout     Request timeout
  */
private[infinity] class InfinityInputStream(
    client: InfinityClient, path: SubPath, bufferSize: Int, timeout: FiniteDuration)
  extends FSInputStream {

  private val context = new StreamContext {
    override def setState(nextState: StreamState): Unit = {
      state = nextState
    }
    override val path: SubPath = InfinityInputStream.this.path
    override def client: InfinityClient = InfinityInputStream.this.client
    override def timeout: FiniteDuration = InfinityInputStream.this.timeout
    override val bufferSize: Int = InfinityInputStream.this.bufferSize
    override var position: Long = 0L
  }
  private var state: StreamState = SeekingStreamState

  override def seek(pos: Long): Unit = {
    state.seek(context, pos)
  }

  override def read(buffer: Array[Byte], offset: Int, length: Int): Int = {
    state.read(context, buffer, offset, length)
  }

  override def read(): Int = { state.read(context) }

  override def close(): Unit = { state.close(context) }

  override def getPos = context.position

  /** Jumping to other copy of the data is not supported. */
  override def seekToNewSource(targetPos: Long): Boolean = false
}
