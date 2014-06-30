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

private object ClosedStreamState extends StreamState {

  override def seek(context: StreamContext, position: Long): Unit = {
    context.position = position
  }

  override def read(context: StreamContext, b: Array[Byte], off: Int, len: Int) = throwError
  override def read(context: StreamContext) = throwError
  override def close(context: StreamContext) = throwError

  private def throwError = throw new IOException("Stream is already closed")
}

